#ifndef SNAKE3_RADARRENDERER_H
#define SNAKE3_RADARRENDERER_H

#include "BaseRenderer.h"
#include "../../ItemsDto/Radar.h"

namespace Renderer {

    class RadarRenderer : public BaseRenderer {
    public:
        explicit RadarRenderer(Radar* radar);
        ~RadarRenderer() override;

    public:
        void render() override;
        void beforeRender() override;
        void afterRender() override;
    protected:
        Radar* radar;
    };

} // Renderer

#endif //SNAKE3_RADARRENDERER_H
