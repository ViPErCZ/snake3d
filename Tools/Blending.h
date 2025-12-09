#ifndef SNAKE3_BLENDING_H
#define SNAKE3_BLENDING_H

namespace Tools {
    enum class Blending {
        /**
         * Opaque material
         */
        Opaque,

        /**
         * Mix based on alpha channel
         *
         * color.rgb * color.a + background.rgb * (1 - color.a)
         */
        Translucent,

        /**
         * Mix colors with addition
         *
         * color.rgb + background.rgb
         */
        Additive,

        /**
         * Mix colors with multiplication
         *
         * color.rgb * background.rgb
         */
        Modulate,

        /**
         * Used for text rendering
         */
        Text
    };
} // Tools

#endif //SNAKE3_BLENDING_H
