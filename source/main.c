#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <pngu.h> // Include the libpngu library for handling PNG images

static u32 *xfb;
static GXRModeObj *rmode;

// Function to load and display PNG images
void LoadImage(const char *imagePath) {
    IMGCTX ctx;
    PNGUPROP imgProp;
    u32 *texture = NULL;

    // Open the PNG file
    ctx = PNGU_SelectImageFromDevice(imagePath);
    if (!ctx) {
        printf("Failed to open image: %s\n", imagePath);
        return;
    }

    // Get image properties
    if (PNGU_GetImageProperties(ctx, &imgProp) != PNGU_OK) {
        printf("Failed to get image properties for: %s\n", imagePath);
        PNGU_ReleaseImageContext(ctx);
        return;
    }

    // Allocate memory for the texture
    texture = (u32 *)memalign(32, imgProp.imgWidth * imgProp.imgHeight * 4);
    if (!texture) {
        printf("Failed to allocate memory for texture\n");
        PNGU_ReleaseImageContext(ctx);
        return;
    }

    // Decode the image into the texture
    if (PNGU_DecodeTo4x4RGBA8(ctx, texture, imgProp.imgWidth, imgProp.imgHeight, 0) != PNGU_OK) {
        printf("Failed to decode image: %s\n", imagePath);
        free(texture);
        PNGU_ReleaseImageContext(ctx);
        return;
    }

    // Display success message
    printf("Successfully loaded image: %s\n", imagePath);

    // Cleanup
    free(texture);
    PNGU_ReleaseImageContext(ctx);
}

void Initialise() {
    VIDEO_Init();
    PAD_Init();

    rmode = VIDEO_GetPreferredMode(NULL);

    xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    console_init(xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);

    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if (rmode->viTVMode & VI_NON_INTERLACE)
        VIDEO_WaitVSync();
}

int main() {
    Initialise();

    printf("Loading images from /apps/AeroRadio/data/ ...\n");

    // Load images
    LoadImage("/apps/AeroRadio/data/bg.png");
    LoadImage("/apps/AeroRadio/data/error.png");
    LoadImage("/apps/AeroRadio/data/error_icon.png");
    LoadImage("/apps/AeroRadio/data/cursor.png");

    printf("Images loaded successfully!\n");

    return 0;
}
