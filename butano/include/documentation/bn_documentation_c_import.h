/*
 * Copyright (c) 2020-2025 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#ifndef BN_DOCUMENTATION_IMPORT_H
#define BN_DOCUMENTATION_IMPORT_H

/**
 * @page import Importing assets
 *
 * @tableofcontents
 *
 * Importing your graphic and audio files into a GBA game can be annoying,
 * but with Butano and this guide you will be good to go.
 *
 * GBA ROMs by themselves don't include a file system, so you can't put a couple of `*.bmp` files into a folder
 * and expect to read them directly from the GBA side.
 *
 * This means that all the game's data has to be added directly to the binary.
 * Don't worry, because Butano build system does this for you.
 * When you drop a file into an assets folder, Butano:
 * * Generates a GBA-friendly version of it.
 * * Inserts it into the ROM.
 * * Creates a C++ header into the `build` folder containing the required information to use the assets with ease.
 *
 * Let's see how to import image and audio files.
 *
 *
 * @section import_image Images
 *
 * By default image files go into the `graphics` folder of your project.
 *
 * Butano for now is a little finicky about the images it likes, sorry.
 *
 * The required image format is the following:
 * * BMP without compression or color space information.
 * * 16 or 256 colors only.
 * * The first color in the color palette is the transparent one, so in most cases it will not be shown on screen.
 *
 * If you get an `invalid header size` error when importing a `*.bmp` file, it probably means that
 * the image format is not supported.
 *
 * If you are using <a href="https://www.gimp.org/">GIMP</a> for your images,
 * remember to disable color space information:
 *
 * @image html import_gimp.png
 *
 * However, the recommended tool to ensure that your images are compatible
 * with Butano is <a href="https://github.com/gb-archive/usenti">Usenti</a>:
 *
 * @image html import_usenti.png
 *
 * Usenti is a simple bitmap editor for paletted images,
 * it is like the good old Paint but with various palette/color manipulation tools.
 *
 * Any program that can generate indexed `*.bmp` files without weird headers should work. However:
 * * Usenti works with 15BPP (bits per pixel) colors, like the GBA and unlike most other popular image editors,
 *   which work with 24BPP or 32BPP colors. BPP reduction destroys some color gradients,
 *   so the images generated by Usenti are closer to what a GBA can display.
 * * Usenti is powered by <a href="https://github.com/devkitPro/grit">grit</a>, the tool used by Butano
 *   to convert images to GBA friendly data. grit doesn't support some `*.bmp` features like
 *   compression or some weird headers, so if Usenti displays a `*.bmp` file without issues,
 *   grit should be able to process it successfully.
 *
 * If you are not going to use Usenti for your images, at least try to check them with it
 * when they are not displayed as expected.
 *
 * A single `*.bmp` file is not enough to display graphics on the GBA.
 * You must accompany it with a `*.json` file with the same name specifying if it is a sprite or a background
 * and some more info.
 *
 * Let's see how to do it.
 *
 *
 * @subsection import_sprite Sprites
 *
 * An image file can contain multiple sprite images.
 * If it only contains one sprite image, its size must be one of the specified by @ref bn::sprite_shape_size.
 *
 * Multiple sprite images are allowed by layering them down on the vertical axis:
 *
 * @image html import_sprite.png
 *
 * An example of the `*.json` files required for sprites is the following:
 *
 * @code{.json}
 * {
 *     "type": "sprite"
 * }
 * @endcode
 *
 * The fields for sprites are the following:
 * * `"type"`: must be `"sprite"` for sprites.
 * * `"height"`: optional field which specifies the height of each sprite image in pixels.
 * For example, if the specified height is 32, an image with 128 pixels of height contains 4 sprite images.
 * * `"bpp_mode"`: optional field which specifies the bits per pixel of the sprite:
 *   * `"bpp_8"`: up to 256 colors.
 *   * `"bpp_4"`: up to 16 colors.
 * * `"colors_count"`: optional field which specifies the sprite palette size [1..256].
 * * `"tiles_compression"`: optional field which specifies the compression of the tiles data:
 *   * `"none"`: uncompressed data (this is the default option).
 *   * `"lz77"`: LZ77 compressed data.
 *   * `"run_length"`: run-length compressed data.
 *   * `"huffman"`: Huffman compressed data.
 *   * `"auto"`: uses the option which gives the smallest data size.
 *   * `"auto_no_huffman"`: uses the option which gives the smallest data size, excluding "huffman".
 * * `"palette_compression"`: optional field which specifies the compression of the colors data:
 *   * `"none"`: uncompressed data (this is the default option).
 *   * `"lz77"`: LZ77 compressed data.
 *   * `"run_length"`: run-length compressed data.
 *   * `"huffman"`: Huffman compressed data.
 *   * `"auto"`: uses the option which gives the smallest data size.
 *   * `"auto_no_huffman"`: uses the option which gives the smallest data size, excluding "huffman".
 * * `"compression"`: optional field which specifies the compression of the tiles and the colors data:
 *   * `"none"`: uncompressed data (this is the default option).
 *   * `"lz77"`: LZ77 compressed data.
 *   * `"run_length"`: run-length compressed data.
 *   * `"huffman"`: Huffman compressed data.
 *   * `"auto"`: uses the option which gives the smallest data size.
 *   * `"auto_no_huffman"`: uses the option which gives the smallest data size, excluding "huffman".
 *
 * If the conversion process has finished successfully,
 * a bn::sprite_item should have been generated in the `build` folder.
 *
 * For example, from two files named `image.bmp` and `image.json`,
 * a header file named `bn_sprite_items_image.h` is generated in the `build` folder.
 *
 * You can use this header to create a sprite with only one line of C++ code:
 *
 * @code{.cpp}
 * #include "bn_sprite_items_image.h"
 *
 * bn::sprite_ptr sprite = bn::sprite_items::image.create_sprite(0, 0);
 * @endcode
 *
 *
 * @subsection import_sprite_tiles Sprite tiles
 *
 * An image file can contain multiple sprite tile sets.
 * If it only contains one sprite tile set,
 * the size of the image must be one of the specified by @ref bn::sprite_shape_size.
 *
 * Multiple sprite tile sets are allowed by layering them down on the vertical axis:
 *
 * @image html import_sprite.png
 *
 * An example of the `*.json` files required for sprite tiles is the following:
 *
 * @code{.json}
 * {
 *     "type": "sprite_tiles"
 * }
 * @endcode
 *
 * The fields for sprite tiles are the following:
 * * `"type"`: must be `"sprite_tiles"` for sprite tiles.
 * * `"height"`: optional field which specifies the height of each sprite tile set in pixels.
 * For example, if the specified height is 32, an image with 128 pixels of height contains 4 sprite tile sets.
 * * `"bpp_mode"`: optional field which specifies the bits per pixel of the sprite tiles:
 *   * `"bpp_8"`: up to 256 colors.
 *   * `"bpp_4"`: up to 16 colors.
 * * `"colors_count"`: optional field which specifies the sprite palette size [1..256].
 * * `"compression"`: optional field which specifies the compression of the tiles data:
 *   * `"none"`: uncompressed data (this is the default option).
 *   * `"lz77"`: LZ77 compressed data.
 *   * `"run_length"`: run-length compressed data.
 *   * `"huffman"`: Huffman compressed data.
 *   * `"auto"`: uses the option which gives the smallest data size.
 *   * `"auto_no_huffman"`: uses the option which gives the smallest data size, excluding "huffman".
 *
 * If the conversion process has finished successfully,
 * a bn::sprite_tiles_item should have been generated in the `build` folder.
 *
 * For example, from two files named `image.bmp` and `image.json`,
 * a header file named `bn_sprite_tiles_items_image.h` is generated in the `build` folder.
 *
 * You can use this header to create sprite tiles with only one line of C++ code:
 *
 * @code{.cpp}
 * #include "bn_sprite_tiles_items_image.h"
 *
 * bn::sprite_tiles_ptr sprite_tiles = bn::sprite_tiles_items::image.create_tiles();
 * @endcode
 *
 *
 * @subsection import_sprite_palette Sprite palettes
 *
 * An example of the `*.json` files required for sprite palettes is the following:
 *
 * @code{.json}
 * {
 *     "type": "sprite_palette"
 * }
 * @endcode
 *
 * The fields for sprite palettes are the following:
 * * `"type"`: must be `"sprite_palette"` for sprite palettes.
 * * `"bpp_mode"`: optional field which specifies the bits per pixel of the sprite palette:
 *   * `"bpp_8"`: up to 256 colors.
 *   * `"bpp_4"`: up to 16 colors.
 * * `"colors_count"`: optional field which specifies the sprite palette size [1..256].
 * * `"compression"`: optional field which specifies the compression of the colors data:
 *   * `"none"`: uncompressed data (this is the default option).
 *   * `"lz77"`: LZ77 compressed data.
 *   * `"run_length"`: run-length compressed data.
 *   * `"huffman"`: Huffman compressed data.
 *   * `"auto"`: uses the option which gives the smallest data size.
 *   * `"auto_no_huffman"`: uses the option which gives the smallest data size, excluding "huffman".
 *
 * If the conversion process has finished successfully,
 * a bn::sprite_palette_item should have been generated in the `build` folder.
 *
 * For example, from two files named `image.bmp` and `image.json`,
 * a header file named `bn_sprite_palette_items_image.h` is generated in the `build` folder.
 *
 * You can use this header to create a sprite palette with only one line of C++ code:
 *
 * @code{.cpp}
 * #include "bn_sprite_palette_items_image.h"
 *
 * bn::sprite_palette_ptr sprite_palette = bn::sprite_palette_items::image.create_palette();
 * @endcode
 *
 *
 * @subsection import_regular_bg Regular backgrounds
 *
 * An image file can contain multiple regular backgrounds.
 * The size of a small regular background (which are faster) must be 256x256, 256x512, 512x256 or 512x512 pixels.
 * Big regular backgrounds are slower CPU wise, but can have any width or height multiple of 256 pixels.
 *
 * Multiple regular background images are allowed by layering them down on the vertical axis:
 *
 * @image html import_regular_bg.png
 *
 * An example of the `*.json` files required for regular backgrounds is the following:
 *
 * @code{.json}
 * {
 *     "type": "regular_bg"
 * }
 * @endcode
 *
 * The fields for regular backgrounds are the following:
 * * `"type"`: must be `"regular_bg"` for regular backgrounds.
 * * `"height"`: optional field which specifies the height of each regular background in pixels.
 * For example, if the specified height is 256, an image with 1024 pixels of height contains 4 regular backgrounds.
 * * `"palette_item"`: optional field which specifies the name of the bn::bg_palette_item to use for this background.
 * * `"bpp_mode"`: optional field which specifies the bits per pixel of the regular background.
 * This field is required if an external bn::bg_palette_item is referenced with `"palette_item"`:
 *   * `"bpp_8"`: up to 256 colors.
 *   * `"bpp_4_auto"`: up to 16 colors per @ref tile "tile".
 * Butano tries to quantize the image to fit the color palette into the required one.
 * It is not supported if an external bn::bg_palette_item is referenced with `"palette_item"`.
 *   * `"bpp_4_manual"`: up to 16 colors per @ref tile "tile".
 * Butano expects that the image color palette is already valid for this mode.
 *   * `"bpp_4"`: `"bpp_4_manual"` alias.
 *
 * The default is `"bpp_4_manual"` for 16 color images and `"bpp_8"` for 256 color images.
 * * `"colors_count"`: optional field which specifies the background palette size [1..256].
 * * `"repeated_tiles_reduction"`: optional field which specifies if repeated tiles must be reduced or not
 * (`true` by default).
 * * `"flipped_tiles_reduction"`: optional field which specifies if flipped tiles must be reduced or not
 * (`true` by default).
 * * `"palette_reduction"`: optional field which specifies if repeated 16 color palettes must be reduced or not
 * (`true` by default).
 * * `"big"`: optional boolean field which specifies if maps generated with this item are big or not.
 *    If this field is omitted, big maps are generated only if needed.
 * * `"tiles_compression"`: optional field which specifies the compression of the tiles data:
 *   * `"none"`: uncompressed data (this is the default option).
 *   * `"lz77"`: LZ77 compressed data.
 *   * `"run_length"`: run-length compressed data.
 *   * `"huffman"`: Huffman compressed data.
 *   * `"auto"`: uses the option which gives the smallest data size.
 *   * `"auto_no_huffman"`: uses the option which gives the smallest data size, excluding "huffman".
 * * `"palette_compression"`: optional field which specifies the compression of the colors data:
 *   * `"none"`: uncompressed data (this is the default option).
 *   * `"lz77"`: LZ77 compressed data.
 *   * `"run_length"`: run-length compressed data.
 *   * `"huffman"`: Huffman compressed data.
 *   * `"auto"`: uses the option which gives the smallest data size.
 *   * `"auto_no_huffman"`: uses the option which gives the smallest data size, excluding "huffman".
 * * `"map_compression"`: optional field which specifies the compression of the map data:
 *   * `"none"`: uncompressed data (this is the default option).
 *   * `"lz77"`: LZ77 compressed data.
 *   * `"run_length"`: run-length compressed data.
 *   * `"huffman"`: Huffman compressed data.
 *   * `"auto"`: uses the option which gives the smallest data size.
 *   * `"auto_no_huffman"`: uses the option which gives the smallest data size, excluding "huffman".
 * * `"compression"`: optional field which specifies the compression of the tiles, the colors and the map data:
 *   * `"none"`: uncompressed data (this is the default option).
 *   * `"lz77"`: LZ77 compressed data.
 *   * `"run_length"`: run-length compressed data.
 *   * `"huffman"`: Huffman compressed data.
 *   * `"auto"`: uses the option which gives the smallest data size.
 *   * `"auto_no_huffman"`: uses the option which gives the smallest data size, excluding "huffman".
 *
 * If the conversion process has finished successfully,
 * a bn::regular_bg_item should have been generated in the `build` folder.
 *
 * For example, from two files named `image.bmp` and `image.json`,
 * a header file named `bn_regular_bg_items_image.h` is generated in the `build` folder.
 *
 * You can use this header to create a regular background with only one line of C++ code:
 *
 * @code{.cpp}
 * #include "bn_regular_bg_items_image.h"
 *
 * bn::regular_bg_ptr regular_bg = bn::regular_bg_items::image.create_bg(0, 0);
 * @endcode
 *
 *
 * @subsection import_regular_bg_tiles Regular background tiles
 *
 * An image file can contain up to 1024 regular background tiles.
 *
 * An example of the `*.json` files required for regular background tiles is the following:
 *
 * @code{.json}
 * {
 *     "type": "regular_bg_tiles",
 *     "bpp_mode": "bpp_8"
 * }
 * @endcode
 *
 * The fields for regular background tiles are the following:
 * * `"type"`: must be `"regular_bg_tiles"` for regular background tiles.
 * * `"bpp_mode"`: specifies the bits per pixel of the regular background tiles:
 *   * `"bpp_8"`: up to 256 colors.
 *   * `"bpp_4"`: up to 16 colors per @ref tile "tile".
 * Butano expects that the image color palette is already valid for this mode.
 * * `"compression"`: optional field which specifies the compression of the tiles data:
 *   * `"none"`: uncompressed data (this is the default option).
 *   * `"lz77"`: LZ77 compressed data.
 *   * `"run_length"`: run-length compressed data.
 *   * `"huffman"`: Huffman compressed data.
 *   * `"auto"`: uses the option which gives the smallest data size.
 *   * `"auto_no_huffman"`: uses the option which gives the smallest data size, excluding "huffman".
 * * `"generate_palette"`: optional field which specifies if a background palette must be generated (`false` by default).
 * * `"palette_colors_count"`: optional field which specifies the background palette size [1..256].
 * * `"palette_compression"`: optional field which specifies the compression of the colors data:
 *   * `"none"`: uncompressed data (this is the default option).
 *   * `"lz77"`: LZ77 compressed data.
 *   * `"run_length"`: run-length compressed data.
 *   * `"huffman"`: Huffman compressed data.
 *   * `"auto"`: uses the option which gives the smallest data size.
 *   * `"auto_no_huffman"`: uses the option which gives the smallest data size, excluding "huffman".
 *
 * If the conversion process has finished successfully,
 * a bn::regular_bg_tiles_item should have been generated in the `build` folder.
 *
 * For example, from two files named `image.bmp` and `image.json`,
 * a header file named `bn_regular_bg_tiles_items_image.h` is generated in the `build` folder.
 *
 * You can use this header to create regular background tiles with only one line of C++ code:
 *
 * @code{.cpp}
 * #include "bn_regular_bg_tiles_items_image.h"
 *
 * bn::regular_bg_tiles_ptr regular_bg_tiles = bn::regular_bg_tiles_items::image.create_tiles();
 * @endcode
 *
 *
 * @subsection import_affine_bg Affine backgrounds
 *
 * An image file can contain multiple affine backgrounds.
 * The size of a small affine background (which are faster) must be 128x128, 256x256, 512x512 or 1024x1024 pixels.
 * Big affine backgrounds are slower CPU wise, but can have any width or height multiple of 256 pixels.
 *
 * Multiple affine background images are allowed by layering them down on the vertical axis:
 *
 * @image html import_affine_bg.png
 *
 * An example of the `*.json` files required for affine backgrounds is the following:
 *
 * @code{.json}
 * {
 *     "type": "affine_bg"
 * }
 * @endcode
 *
 * The fields for affine backgrounds are the following:
 * * `"type"`: must be `"affine_bg"` for affine backgrounds.
 * * `"height"`: optional field which specifies the height of each affine background in pixels.
 * For example, if the specified height is 256, an image with 1024 pixels of height contains 4 affine backgrounds.
 * * `"palette_item"`: optional field which specifies the name of the bn::bg_palette_item to use for this background.
 * * `"colors_count"`: optional field which specifies the background palette size [1..256].
 * * `"repeated_tiles_reduction"`: optional field which specifies if repeated tiles must be reduced or not
 * (`true` by default).
 * * `"big"`: optional boolean field which specifies if maps generated with this item are big or not.
 *    If this field is omitted, big maps are generated only if needed.
 * * `"tiles_compression"`: optional field which specifies the compression of the tiles data:
 *   * `"none"`: uncompressed data (this is the default option).
 *   * `"lz77"`: LZ77 compressed data.
 *   * `"run_length"`: run-length compressed data.
 *   * `"huffman"`: Huffman compressed data.
 *   * `"auto"`: uses the option which gives the smallest data size.
 *   * `"auto_no_huffman"`: uses the option which gives the smallest data size, excluding "huffman".
 * * `"palette_compression"`: optional field which specifies the compression of the colors data:
 *   * `"none"`: uncompressed data (this is the default option).
 *   * `"lz77"`: LZ77 compressed data.
 *   * `"run_length"`: run-length compressed data.
 *   * `"huffman"`: Huffman compressed data.
 *   * `"auto"`: uses the option which gives the smallest data size.
 *   * `"auto_no_huffman"`: uses the option which gives the smallest data size, excluding "huffman".
 * * `"map_compression"`: optional field which specifies the compression of the map data:
 *   * `"none"`: uncompressed data (this is the default option).
 *   * `"lz77"`: LZ77 compressed data.
 *   * `"run_length"`: run-length compressed data.
 *   * `"huffman"`: Huffman compressed data.
 *   * `"auto"`: uses the option which gives the smallest data size.
 *   * `"auto_no_huffman"`: uses the option which gives the smallest data size, excluding "huffman".
 * * `"compression"`: optional field which specifies the compression of the tiles, the colors and the map data:
 *   * `"none"`: uncompressed data (this is the default option).
 *   * `"lz77"`: LZ77 compressed data.
 *   * `"run_length"`: run-length compressed data.
 *   * `"huffman"`: Huffman compressed data.
 *   * `"auto"`: uses the option which gives the smallest data size.
 *   * `"auto_no_huffman"`: uses the option which gives the smallest data size, excluding "huffman".
 *
 * If the conversion process has finished successfully,
 * a bn::affine_bg_item should have been generated in the `build` folder.
 *
 * For example, from two files named `image.bmp` and `image.json`,
 * a header file named `bn_affine_bg_items_image.h` is generated in the `build` folder.
 *
 * You can use this header to create an affine background with only one line of C++ code:
 *
 * @code{.cpp}
 * #include "bn_affine_bg_items_image.h"
 *
 * bn::affine_bg_ptr affine_bg = bn::affine_bg_items::image.create_bg(0, 0);
 * @endcode
 *
 *
 * @subsection import_affine_bg_tiles Affine background tiles
 *
 * An image file can contain up to 256 affine background tiles.
 *
 * An example of the `*.json` files required for affine background tiles is the following:
 *
 * @code{.json}
 * {
 *     "type": "affine_bg_tiles"
 * }
 * @endcode
 *
 * The fields for affine background tiles are the following:
 * * `"type"`: must be `"affine_bg_tiles"` for affine background tiles.
 * * `"compression"`: optional field which specifies the compression of the tiles data:
 *   * `"none"`: uncompressed data (this is the default option).
 *   * `"lz77"`: LZ77 compressed data.
 *   * `"run_length"`: run-length compressed data.
 *   * `"huffman"`: Huffman compressed data.
 *   * `"auto"`: uses the option which gives the smallest data size.
 *   * `"auto_no_huffman"`: uses the option which gives the smallest data size, excluding "huffman".
 * * `"generate_palette"`: optional field which specifies if a background palette must be generated (`false` by default).
 * * `"palette_colors_count"`: optional field which specifies the background palette size [1..256].
 * * `"palette_compression"`: optional field which specifies the compression of the colors data:
 *   * `"none"`: uncompressed data (this is the default option).
 *   * `"lz77"`: LZ77 compressed data.
 *   * `"run_length"`: run-length compressed data.
 *   * `"huffman"`: Huffman compressed data.
 *   * `"auto"`: uses the option which gives the smallest data size.
 *   * `"auto_no_huffman"`: uses the option which gives the smallest data size, excluding "huffman".
 *
 * If the conversion process has finished successfully,
 * a bn::affine_bg_tiles_item should have been generated in the `build` folder.
 *
 * For example, from two files named `image.bmp` and `image.json`,
 * a header file named `bn_affine_bg_tiles_items_image.h` is generated in the `build` folder.
 *
 * You can use this header to create affine background tiles with only one line of C++ code:
 *
 * @code{.cpp}
 * #include "bn_affine_bg_tiles_items_image.h"
 *
 * bn::affine_bg_tiles_ptr affine_bg_tiles = bn::affine_bg_tiles_items::image.create_tiles();
 * @endcode
 *
 *
 * @subsection import_bg_palette Background palettes
 *
 * An example of the `*.json` files required for background palettes is the following:
 *
 * @code{.json}
 * {
 *     "type": "bg_palette",
 *     "bpp_mode": "bpp_8"
 * }
 * @endcode
 *
 * The fields for background palettes are the following:
 * * `"type"`: must be `"bg_palette"` for background palettes.
 * * `"bpp_mode"`: specifies the bits per pixel of the background palette:
 *   * `"bpp_8"`: up to 256 colors.
 *   * `"bpp_4"`: up to 16 colors per @ref tile "tile".
 * * `"colors_count"`: optional field which specifies the background palette size [1..256].
 * * `"compression"`: optional field which specifies the compression of the colors data:
 *   * `"none"`: uncompressed data (this is the default option).
 *   * `"lz77"`: LZ77 compressed data.
 *   * `"run_length"`: run-length compressed data.
 *   * `"huffman"`: Huffman compressed data.
 *   * `"auto"`: uses the option which gives the smallest data size.
 *   * `"auto_no_huffman"`: uses the option which gives the smallest data size, excluding "huffman".
 *
 * If the conversion process has finished successfully,
 * a bn::bg_palette_item should have been generated in the `build` folder.
 *
 * For example, from two files named `image.bmp` and `image.json`,
 * a header file named `bn_bg_palette_items_image.h` is generated in the `build` folder.
 *
 * You can use this header to create a background palette with only one line of C++ code:
 *
 * @code{.cpp}
 * #include "bn_bg_palette_items_image.h"
 *
 * bn::bg_palette_ptr bg_palette = bn::bg_palette_items::image.create_palette();
 * @endcode
 *
 *
 * @section import_audio Audio
 *
 * By default audio files played with Direct Sound channels go into the `audio` folder of your project,
 * and music files played with DMG channels go into the `dmg_audio` folder.
 *
 * Direct Sound stuff is managed by the awesome <a href="https://maxmod.devkitpro.org/">Maxmod</a>,
 * so if you have an issue with Direct Sound music or with sound effects, well, you know.
 *
 * DMG channels are handled by the also impressive <a href="https://github.com/AntonioND/gbt-player">GBT Player</a>
 * and <a href="https://github.com/copyrat90/gbadev-ja-test">VGM player</a>,
 * so if you have issues with DMG music, you know where to go.
 *
 * A really nice application for editing audio files
 * before importing them into your game is <a href="https://openmpt.org/">OpenMPT</a>.
 *
 *
 * @subsection import_direct_sound_music Direct Sound music
 *
 * The required format for Direct Sound music is module files with `*.mod`, `*.xm`, `*.s3m` and `*.it` extensions.
 *
 * By default Butano supports up to 16 music channels,
 * but this limit can be increased by overloading the definition of @ref BN_CFG_AUDIO_MAX_MUSIC_CHANNELS.
 *
 * However, if it is possible don't do this, don't make the poor GBA suffer.
 *
 * If the conversion process has finished successfully,
 * a bunch of bn::music_item objects under the `bn::music_items` namespace
 * should have been generated in the `build` folder for all music files.
 * You can use these items to play music with only one line of C++ code:
 *
 * @code{.cpp}
 * #include "bn_music_items.h"
 *
 * bn::music_items::song.play();
 * @endcode
 *
 *
 * @subsection import_dmg_music DMG music
 *
 * The required format for DMG music is module files with `*.mod`, `*.s3m` and `*.vgm` extensions.
 *
 * The recommended way to generate `*.vgm` compatible files is to use
 * <a href="https://github.com/SuperDisk/hUGETracker">hUGETracker 1.01</a> with timer based tempo disabled:
 *
 * @image html import_hugetracker.png
 *
 * If the conversion process has finished successfully,
 * a bn::dmg_music_item should have been generated in the `build` folder.
 *
 * For example, from a file named `module.mod`,
 * a header file named `bn_dmg_music_items_module.h` is generated in the `build` folder.
 *
 * You can use this header to play the module file with only one line of C++ code:
 *
 * @code{.cpp}
 * #include "bn_dmg_music_items_module.h"
 *
 * bn::dmg_music_items::module.play();
 * @endcode
 *
 * You can accompany DMG music module files with a `*.json` file with the same name to specify import options.
 *
 * An example of the `*.json` files for DMG music module files is the following:
 *
 * @code{.json}
 * {
 *     "import_instruments": false,
 *     "mod_speed_conversion": true
 * }
 * @endcode
 *
 * Available fields are the following:
 * * `"import_instruments"`: optional field which specifies if channel 3 instruments must be imported
 * when importing `*.s3m` module files, replacing the default instruments of GBT Player (`false` by default).
 * This option is ignored when importing audio files with `*.mod` and `*.vgm` extensions.
 * * `"mod_speed_conversion"`: optional field which specifies if module files with `*.mod` extension speed
 * must be converted from 50Hz to 60Hz (`true` by default). This option is ignored when importing audio files
 * with `*.s3m` and `*.vgm` extensions.
 *
 * The default DMG music master volume is set to 25% ( bn::dmg_music_master_volume::QUARTER ).
 * If it sounds too quiet for you, you can change it via bn::dmg_music::set_master_volume.
 *
 *
 * @subsection import_sound Sound effects
 *
 * The required format for sound effects is waveform audio files (files with `*.wav` extension)
 * without compression or anything weird. Besides, *I think* stereo files are not allowed.
 *
 * The recommended quality for sound effects is 8-bits 22050 Hz.
 *
 * If the conversion process has finished successfully,
 * a bunch of bn::sound_item objects under the `bn::sound_items` namespace
 * should have been generated in the `build` folder for all sound files.
 * You can use these items to play sound effects with only one line of C++ code:
 *
 * @code{.cpp}
 * #include "bn_sound_items.h"
 *
 * bn::sound_items::sfx.play();
 * @endcode
 */

#endif
