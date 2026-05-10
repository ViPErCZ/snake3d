package main

import (
    "encoding/json"
    "fmt"
    "io/fs"
    "os"
    "path/filepath"
)

type TextureEntry struct {
    Name     string `json:"name"`
    Path     string `json:"path"`
    Category string `json:"category"`
}

func main() {
    paths := map[string]string{
        "Albedo": "Assets/Textures/Albedo",
        "Others": "Assets/Textures/Others",
    }

    var manifest []TextureEntry

    for category, folder := range paths {
        err := filepath.WalkDir(folder, func(path string, d fs.DirEntry, err error) error {
            if err != nil {
                fmt.Fprintf(os.Stderr, "Chyba při čtení %s: %v\n", path, err)
                return nil
            }
            if !d.IsDir() {
                relPath, _ := filepath.Rel("Assets/Textures", path)
                entry := TextureEntry{
                    Name:     d.Name(),
                    Path:     filepath.ToSlash(relPath),
                    Category: category,
                }
                manifest = append(manifest, entry)
            }
            return nil
        })
        if err != nil {
            fmt.Fprintf(os.Stderr, "Chyba při průchodu složkou %s: %v\n", folder, err)
        }
    }

    file, err := os.Create("texture_manifest.json")
    if err != nil {
        panic(err)
    }
    defer file.Close()

    enc := json.NewEncoder(file)
    enc.SetIndent("", "  ")
    if err := enc.Encode(manifest); err != nil {
        panic(err)
    }

    fmt.Printf("Manifest vytvořen: %d položek\n", len(manifest))
}
