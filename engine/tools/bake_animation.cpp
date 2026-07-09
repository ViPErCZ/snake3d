// bake_animation — offline skinned-animation → per-frame OBJ "flipbook" baker.
//
// Loads a skeletal model + an animation through assimp, evaluates the linear-blend skin on
// the CPU at evenly-spaced ticks across the clip, and writes one OBJ mesh per frame
// (<prefix>_NN.obj). Playing those static meshes back in sequence ("flipbook") reproduces the
// animation without any runtime skinning — useful for cheap decorative actors, or for rigs
// whose live skinning is unreliable.
//
// The MESH and ANIMATION may come from SEPARATE files (pass the same path twice for a
// self-contained rig). This matters for assets whose animation file has an inconsistent
// internal bind pose (where generic LBS collapses the mesh into a blob): point <mesh.dae> at
// a file with a clean, consistent bind, and <anim.dae> at the clip — the skin uses the mesh
// file's geometry + bones + offset (inverse-bind) + rest hierarchy, and grafts the per-frame
// joint motion from the animation file, matched by bone NAME. assimp bakes each file's
// bind-shape matrix into its vertices/offsets and normalises units, so this resolves bind/unit
// mismatches that defeat a hand-rolled Collada skinner. (This is essentially how engines that
// share one skeletal mesh across many separate animation clips combine them.)
//
// Build (opt-in): cmake -DBUILD_ENGINE_TOOLS=ON ...   then target `bake_animation`.
// Standalone:     g++ -O2 -std=c++17 bake_animation.cpp -lassimp -o bake_animation
// Run:  bake_animation <mesh.dae> <anim.dae> <out_dir> <prefix> [maxFrames=24]
//         -> <out_dir>/<prefix>_NN.obj  (OBJ carries positions + UVs; normals are recomputed
//            by whatever loads them). Pass the same path for <mesh.dae> and <anim.dae> when the
//            rig is self-contained.
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <functional>
#include <map>
#include <string>
#include <vector>

// Interpolated local transform of an animated node at clip time t (T * R * S).
static aiMatrix4x4 nodeLocalAt(const aiNodeAnim* ch, double t) {
    aiVector3D pos, scl(1, 1, 1); aiQuaternion rot;
    if (ch) {
        auto interpV = [&](unsigned n, auto keys) {
            aiVector3D v = keys[0].mValue;
            for (unsigned i = 0; i + 1 < n; ++i)
                if (t >= keys[i].mTime && t <= keys[i + 1].mTime) {
                    double f = (t - keys[i].mTime) / (keys[i + 1].mTime - keys[i].mTime + 1e-12);
                    v = keys[i].mValue + (keys[i + 1].mValue - keys[i].mValue) * (float)f; break;
                } else if (t > keys[n - 1].mTime) v = keys[n - 1].mValue;
            return v;
        };
        if (ch->mNumPositionKeys) pos = interpV(ch->mNumPositionKeys, ch->mPositionKeys);
        if (ch->mNumScalingKeys)  scl = interpV(ch->mNumScalingKeys,  ch->mScalingKeys);
        if (ch->mNumRotationKeys) {
            rot = ch->mRotationKeys[0].mValue;
            for (unsigned i = 0; i + 1 < ch->mNumRotationKeys; ++i)
                if (t >= ch->mRotationKeys[i].mTime && t <= ch->mRotationKeys[i + 1].mTime) {
                    double f = (t - ch->mRotationKeys[i].mTime) /
                               (ch->mRotationKeys[i + 1].mTime - ch->mRotationKeys[i].mTime + 1e-12);
                    aiQuaternion::Interpolate(rot, ch->mRotationKeys[i].mValue,
                                              ch->mRotationKeys[i + 1].mValue, (float)f); rot.Normalize(); break;
                } else if (t > ch->mRotationKeys[ch->mNumRotationKeys - 1].mTime)
                    rot = ch->mRotationKeys[ch->mNumRotationKeys - 1].mValue;
        }
    }
    aiMatrix4x4 T, S; aiMatrix4x4::Translation(pos, T); aiMatrix4x4::Scaling(scl, S);
    return T * aiMatrix4x4(rot.GetMatrix()) * S;
}

int main(int argc, char** argv) {
    if (argc < 5) {
        fprintf(stderr, "usage: bake_animation <mesh.dae> <anim.dae> <out_dir> <prefix> [maxFrames=24]\n"
                        "       (pass the same path for <mesh.dae> and <anim.dae> if self-contained)\n");
        return 1;
    }
    const std::string meshFile = argv[1], animFile = argv[2], outDir = argv[3], prefix = argv[4];
    const int maxFrames = argc > 5 ? std::max(1, atoi(argv[5])) : 24;
    std::filesystem::create_directories(outDir);

    Assimp::Importer impMesh, impAnim;
    const aiScene* sc = impMesh.ReadFile(meshFile, aiProcess_Triangulate);
    if (!sc) { fprintf(stderr, "mesh load failed: %s\n", impMesh.GetErrorString()); return 1; }
    const aiScene* sca = impAnim.ReadFile(animFile, aiProcess_Triangulate);
    if (!sca) { fprintf(stderr, "anim load failed: %s\n", impAnim.GetErrorString()); return 1; }

    // Skinned mesh = the one with the most bones (skip helper geometries).
    const aiMesh* mesh = nullptr;
    for (unsigned i = 0; i < sc->mNumMeshes; ++i)
        if (!mesh || sc->mMeshes[i]->mNumBones > mesh->mNumBones) mesh = sc->mMeshes[i];
    if (!mesh || !mesh->mNumBones) { fprintf(stderr, "no skinned mesh in %s\n", meshFile.c_str()); return 1; }

    // Mesh-file node hierarchy (the rest/bind) + global inverse.
    std::map<std::string, const aiNode*> nodes;
    std::function<void(const aiNode*)> walk = [&](const aiNode* n) {
        nodes[n->mName.C_Str()] = n;
        for (unsigned i = 0; i < n->mNumChildren; ++i) walk(n->mChildren[i]);
    };
    walk(sc->mRootNode);
    aiMatrix4x4 globalInv = sc->mRootNode->mTransformation; globalInv.Inverse();

    // Bone offset (inverse-bind) matrices + per-vertex weights, from the MESH file.
    std::map<std::string, aiMatrix4x4> offset;
    std::vector<std::vector<std::pair<std::string, float>>> w(mesh->mNumVertices);
    for (unsigned b = 0; b < mesh->mNumBones; ++b) {
        const aiBone* bone = mesh->mBones[b];
        offset[bone->mName.C_Str()] = bone->mOffsetMatrix;
        for (unsigned k = 0; k < bone->mNumWeights; ++k)
            w[bone->mWeights[k].mVertexId].push_back({bone->mName.C_Str(), bone->mWeights[k].mWeight});
    }

    // Animation channels (per node) from the ANIMATION file, keyed by bone name.
    const aiAnimation* anim = sca->mNumAnimations ? sca->mAnimations[0] : nullptr;
    std::map<std::string, const aiNodeAnim*> chan;
    if (anim) for (unsigned i = 0; i < anim->mNumChannels; ++i)
        chan[anim->mChannels[i]->mNodeName.C_Str()] = anim->mChannels[i];
    const double dur = anim ? anim->mDuration : 0.0;
    const int nframes = anim ? std::min(maxFrames, std::max(2, (int)dur + 1)) : 1;
    printf("verts=%u faces=%u bones=%u frames=%d dur=%.1f tps=%.1f\n",
           mesh->mNumVertices, mesh->mNumFaces, mesh->mNumBones, nframes,
           dur, anim ? anim->mTicksPerSecond : 0.0);

    // Per-frame global transform of a node: animation channel if present, else mesh rest.
    std::map<std::string, aiMatrix4x4> gcache;
    std::function<aiMatrix4x4(const aiNode*, double)> globalOf =
        [&](const aiNode* n, double t) -> aiMatrix4x4 {
            const std::string nm = n->mName.C_Str();
            auto it = gcache.find(nm); if (it != gcache.end()) return it->second;
            auto ci = chan.find(nm);
            aiMatrix4x4 local = (ci != chan.end()) ? nodeLocalAt(ci->second, t) : n->mTransformation;
            aiMatrix4x4 g = n->mParent ? globalOf(n->mParent, t) * local : local;
            gcache[nm] = g; return g;
        };

    for (int fi = 0; fi < nframes; ++fi) {
        gcache.clear();
        const double t = (nframes > 1) ? (double)fi / (nframes - 1) * dur : 0.0;
        std::map<std::string, aiMatrix4x4> fin;
        for (auto& kv : offset) {
            auto ni = nodes.find(kv.first);
            if (ni == nodes.end() || !ni->second) { fin[kv.first] = kv.second; continue; } // bone has no node
            fin[kv.first] = globalInv * globalOf(ni->second, t) * kv.second;
        }
        char path[1024]; snprintf(path, sizeof path, "%s/%s_%02d.obj", outDir.c_str(), prefix.c_str(), fi);
        FILE* f = fopen(path, "w");
        if (!f) { fprintf(stderr, "cannot write %s\n", path); return 1; }
        const bool hasUV = mesh->mTextureCoords[0] != nullptr;
        for (unsigned vi = 0; vi < mesh->mNumVertices; ++vi) {
            aiVector3D p = mesh->mVertices[vi], acc(0, 0, 0); float tw = 0;
            for (auto& bw : w[vi]) { acc += (fin[bw.first] * p) * bw.second; tw += bw.second; }
            if (tw > 1e-6f) acc /= tw; else acc = p; // unweighted vertex -> leave at rest
            fprintf(f, "v %.4f %.4f %.4f\n", acc.x, acc.y, acc.z);
        }
        if (hasUV) for (unsigned vi = 0; vi < mesh->mNumVertices; ++vi)
            fprintf(f, "vt %.6f %.6f\n", mesh->mTextureCoords[0][vi].x, mesh->mTextureCoords[0][vi].y);
        for (unsigned fa = 0; fa < mesh->mNumFaces; ++fa) {
            const aiFace& face = mesh->mFaces[fa];
            if (face.mNumIndices != 3) continue;
            if (hasUV) fprintf(f, "f %u/%u %u/%u %u/%u\n", face.mIndices[0] + 1, face.mIndices[0] + 1,
                               face.mIndices[1] + 1, face.mIndices[1] + 1, face.mIndices[2] + 1, face.mIndices[2] + 1);
            else fprintf(f, "f %u %u %u\n", face.mIndices[0] + 1, face.mIndices[1] + 1, face.mIndices[2] + 1);
        }
        fclose(f);
    }
    printf("wrote %d frames to %s/%s_NN.obj\n", nframes, outDir.c_str(), prefix.c_str());
    return 0;
}
