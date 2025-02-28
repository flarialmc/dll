#include "SkinStealCommand.hpp"

#include <Utils/WinrtUtils.hpp>

#ifndef SVPNG_LINKAGE
#define SVPNG_LINKAGE
#endif

/*! \def SVPNG_OUTPUT
    \brief User customizable output stream.
    By default, it uses C file descriptor and fputc() to output bytes.
    In C++, for example, user may use std::ostream or std::vector instead.
*/
#ifndef SVPNG_OUTPUT
#include <stdio.h>
#define SVPNG_OUTPUT FILE* fp
#endif
#include "../../../Utils/Utils.hpp"
#include "../../../Utils/Logger/Logger.hpp"
//#include "../json/MinecraftJson.h"
#include <fstream>

/*! \def SVPNG_PUT
    \brief Write a byte
*/
#ifndef SVPNG_PUT
#define SVPNG_PUT(u) fputc(u, fp)
#endif

void svpng(SVPNG_OUTPUT, unsigned w, unsigned h, const unsigned char* img, int alpha) {
    static const unsigned t[] = { 0, 0x1db71064, 0x3b6e20c8, 0x26d930ac, 0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
        /* CRC32 Table */    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c, 0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c };
    unsigned a = 1, b = 0, c, p = w * (alpha ? 4 : 3) + 1, x, y, i;   /* ADLER-a, ADLER-b, CRC, pitch */
#define SVPNG_U8A(ua, l) for (i = 0; i < l; i++) SVPNG_PUT((ua)[i]);
#define SVPNG_U32(u) do { SVPNG_PUT((u) >> 24); SVPNG_PUT(((u) >> 16) & 255); SVPNG_PUT(((u) >> 8) & 255); SVPNG_PUT((u) & 255); } while(0)
#define SVPNG_U8C(u) do { SVPNG_PUT(u); c ^= (u); c = (c >> 4) ^ t[c & 15]; c = (c >> 4) ^ t[c & 15]; } while(0)
#define SVPNG_U8AC(ua, l) for (i = 0; i < l; i++) SVPNG_U8C((ua)[i])
#define SVPNG_U16LC(u) do { SVPNG_U8C((u) & 255); SVPNG_U8C(((u) >> 8) & 255); } while(0)
#define SVPNG_U32C(u) do { SVPNG_U8C((u) >> 24); SVPNG_U8C(((u) >> 16) & 255); SVPNG_U8C(((u) >> 8) & 255); SVPNG_U8C((u) & 255); } while(0)
#define SVPNG_U8ADLER(u) do { SVPNG_U8C(u); a = (a + (u)) % 65521; b = (b + a) % 65521; } while(0)
#define SVPNG_BEGIN(s, l) do { SVPNG_U32(l); c = ~0U; SVPNG_U8AC(s, 4); } while(0)
#define SVPNG_END() SVPNG_U32(~c)
    SVPNG_U8A("\x89PNG\r\n\32\n", 8);           /* Magic */
    SVPNG_BEGIN("IHDR", 13);                    /* IHDR chunk { */
    SVPNG_U32C(w); SVPNG_U32C(h);               /*   Width & Height (8 bytes) */
    SVPNG_U8C(8); SVPNG_U8C(alpha ? 6 : 2);     /*   Depth=8, Color=True color with/without alpha (2 bytes) */
    SVPNG_U8AC("\0\0\0", 3);                    /*   Compression=Deflate, Filter=No, Interlace=No (3 bytes) */
    SVPNG_END();                                /* } */
    SVPNG_BEGIN("IDAT", 2 + h * (5 + p) + 4);   /* IDAT chunk { */
    SVPNG_U8AC("\x78\1", 2);                    /*   Deflate block begin (2 bytes) */
    for (y = 0; y < h; y++) {                   /*   Each horizontal line makes a block for simplicity */
        SVPNG_U8C(y == h - 1);                  /*   1 for the last block, 0 for others (1 byte) */
        SVPNG_U16LC(p); SVPNG_U16LC(~p);        /*   Size of block in little endian and its 1's complement (4 bytes) */
        SVPNG_U8ADLER(0);                       /*   No filter prefix (1 byte) */
        for (x = 0; x < p - 1; x++, img++)
            SVPNG_U8ADLER(*img);                /*   Image pixel data */
    }
    SVPNG_U32C((b << 16) | a);                  /*   Deflate block end with adler (4 bytes) */
    SVPNG_END();                                /* } */
    SVPNG_BEGIN("IEND", 0); SVPNG_END();        /* IEND chunk {} */
};

void saveImage(const char* filename, const unsigned char* img, int width, int height) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        std::string log = "Error: Failed to open file for writing: ", filename;
        Logger::debug(log);
        return;
    }
    svpng(fp, width, height, img, 1); // 0 means no alpha channel
    fclose(fp);
}

void SaveSkin(std::string name, Image image, Image capeImage) {
    const auto imageBytes = image.mImageBytes.data();
    std::filesystem::path folder_path(Utils::getRoamingPath() + "\\Flarial\\Skins");
    if (!exists(folder_path))
    {
        create_directory(folder_path);
    }


    std::string path = Utils::getRoamingPath() + "\\Flarial\\Skins\\" + name + ".png";


    saveImage(path.c_str(), imageBytes, image.mWidth, image.mHeight);

    if (capeImage.mWidth >= 64 && capeImage.mHeight >= 32) {
        const auto capeImageBytes = capeImage.mImageBytes.data();
        std::string path_to_cape = Utils::getRoamingPath() + "\\Flarial\\Skins\\" + name + "_cape.png";
        saveImage(path_to_cape.c_str(), capeImageBytes, capeImage.mWidth, capeImage.mHeight);
    }

    Logger::debug("Image saved successfully.");

    /*
    std::string path_to_geo = Utils::getRoamingPath() + Utils::removeNonAlphanumeric(Utils::removeColorCodes(name)) + "_geo.json";
    std::ofstream geoFile;
    std::string out;
    geoFile.open(path_to_geo);
    geoFile << this->mGeometryData.toStyledString(&out)->c_str();
    geoFile.close();
    */

    Logger::debug("Skin saved successfully.");
}


void SkinStealCommand::execute(const std::vector<std::string>& args) {
    if (args.size() != 1) {
        addCommandMessage("Usage: .steal <playerName>");
        return;
    }
    std::string playerName = String::toLower(args[0]);
    if (playerName == "path") {
        WinrtUtils::openSubFolder("Flarial\\Skins");
        return;
    }
    for (const auto& pair : SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap()) {
        if (String::toLower(pair.second.name) == playerName) {
            addCommandMessage("Succesfully stole {}'s skin!", pair.second.name);
            SaveSkin(pair.second.name, pair.second.playerSkin.mSkinImage, pair.second.playerSkin.mCapeImage);
        }
    }
}