/**
 * HIGH NOON DUEL - 1985 Arcade Style Western
 * A minimalistic Win32 API game written in pure C.
 * Features state machine, CRT scanline effects, and dynamic difficulty.
 * @author d7main
 */

#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

 // Config & Constants
#define WIN_WIDTH 800
#define WIN_HEIGHT 400
#define GROUND_Y 300
#define FPS_DELAY 16

// Color Palette (80s Arcade)
#define COL_SKY       RGB(220, 80, 30)
#define COL_SUN       RGB(255, 200, 0)
#define COL_SAND      RGB(180, 100, 40)
#define COL_SHERIFF   RGB(0, 120, 200)
#define COL_BANDIT    RGB(200, 30, 30)
#define COL_HAT       RGB(80, 40, 10)
#define COL_CACTUS    RGB(40, 140, 40)
#define COL_BULLET    RGB(255, 255, 255)
#define COL_UI_BG     RGB(15, 15, 20)
#define COL_UI_ACCENT RGB(0, 255, 255)
#define COL_DUST      RGB(235, 150, 70) 

// Game System Enumerations
typedef enum {
    STATE_MENU,
    STATE_PLAY,
    STATE_OVER
} GameState;

typedef enum {
    AMMO_NORMAL,
    AMMO_FAST,
    AMMO_HEAVY
} AmmoType;

// Data Structures 
typedef struct {
    float x, y, dy;
    int width, height;
    int isDucking, isJumping, isAlive;
} Entity;

typedef struct {
    float x, y, dx;
    int isActive;
} Projectile;

typedef struct {
    float x, y, dx, dy;
    int life;
} Particle;

// Effects State
Particle dust[20] = { 0 };
Particle sparks[15] = { 0 };
int pFlash = 0;
int eFlash = 0;

// Global Game State
Entity sheriff;
Entity bandit;
Projectile pBullet = { 0 };
Projectile eBullet = { 0 };

GameState gameState = STATE_MENU;
AmmoType currentAmmo = AMMO_NORMAL;

int playerHP = 3;
int enemyHP = 3;
int currentScore = 0;
int highScore = 0;
int frameCounter = 0;
int enemyFireDelay = 50;
int respawnTimer = 0;

// Game Logic Functions
void SpawnBandit() {
    bandit.x = 700;
    bandit.y = GROUND_Y - 60;
    bandit.width = 24;
    bandit.height = 60;
    bandit.isAlive = 1;
    bandit.isDucking = 0;
    bandit.dy = 0;
}

void ResetGameSession() {
    playerHP = 3;
    enemyHP = 3;
    currentAmmo = AMMO_NORMAL;

    sheriff.x = 50;
    sheriff.y = GROUND_Y - 60;
    sheriff.width = 24;
    sheriff.height = 60;
    sheriff.isAlive = 1;
    sheriff.isDucking = 0;
    sheriff.isJumping = 0;
    sheriff.dy = 0;

    pBullet.isActive = 0;
    eBullet.isActive = 0;
    currentScore = 0;
    enemyFireDelay = 50;
    respawnTimer = 0;

    SpawnBandit();


    for (int i = 0; i < 20; i++) {
        dust[i].x = (float)(rand() % WIN_WIDTH);
        dust[i].y = (float)(rand() % (WIN_HEIGHT - 40));
        dust[i].dx = -4.0f - (rand() % 4);
        dust[i].life = 80 + (rand() % 60);
    }
}

void UpdatePhysicsAndLogic() {
    if (GetAsyncKeyState('1') & 0x8000) currentAmmo = AMMO_NORMAL;
    if (GetAsyncKeyState('2') & 0x8000) currentAmmo = AMMO_FAST;
    if (GetAsyncKeyState('3') & 0x8000) currentAmmo = AMMO_HEAVY;

    if (sheriff.isJumping) {
        sheriff.dy += 1.2f;
        sheriff.y += sheriff.dy;
        if (sheriff.y >= GROUND_Y - sheriff.height) {
            sheriff.y = GROUND_Y - sheriff.height;
            sheriff.isJumping = 0;
            sheriff.dy = 0;
        }
    }

    if (bandit.isAlive) {
        if (frameCounter % 40 == 0) {
            bandit.isDucking = (rand() % 3 == 0) ? 1 : 0;
        }
        if (!eBullet.isActive && (rand() % enemyFireDelay == 0)) {
            eBullet.x = bandit.x;
            eBullet.y = bandit.isDucking ? bandit.y + 40 : bandit.y + 20;
            eBullet.dx = -12.0f - (currentScore / 500.0f);
            eBullet.isActive = 1;
            eFlash = 4;
        }
    }

    if (pBullet.isActive) {
        pBullet.x += pBullet.dx;
        if (pBullet.x > WIN_WIDTH) pBullet.isActive = 0;

        int hitBoxTop = bandit.isDucking ? bandit.y + 30 : bandit.y;
        if (bandit.isAlive && pBullet.x >= bandit.x && pBullet.x <= bandit.x + bandit.width &&
            pBullet.y >= hitBoxTop && pBullet.y <= bandit.y + bandit.height) {

            pBullet.isActive = 0;
            int damage = (currentAmmo == AMMO_HEAVY) ? 2 : 1;
            enemyHP -= damage;

            for (int i = 0; i < 15; i++) {
                sparks[i].x = bandit.x;
                sparks[i].y = pBullet.y;
                sparks[i].dx = (float)((rand() % 10) - 5);
                sparks[i].dy = (float)((rand() % 10) - 5);
                sparks[i].life = 10 + (rand() % 10);
            }

            if (enemyHP <= 0) {
                bandit.isAlive = 0;
                currentScore += 100;
                respawnTimer = 35;
                enemyHP = 3;
                if (enemyFireDelay > 15) enemyFireDelay -= 3;
            }
        }
    }

    if (eBullet.isActive) {
        eBullet.x += eBullet.dx;
        if (eBullet.x < 0) eBullet.isActive = 0;

        int hitBoxTop = sheriff.isDucking ? sheriff.y + 30 : sheriff.y;
        if (eBullet.x >= sheriff.x && eBullet.x <= sheriff.x + sheriff.width &&
            eBullet.y >= hitBoxTop && eBullet.y <= sheriff.y + sheriff.height) {

            eBullet.isActive = 0;
            playerHP -= 1;

            for (int i = 0; i < 15; i++) {
                sparks[i].x = sheriff.x + sheriff.width;
                sparks[i].y = eBullet.y;
                sparks[i].dx = (float)((rand() % 10) - 5);
                sparks[i].dy = (float)((rand() % 10) - 5);
                sparks[i].life = 10 + (rand() % 10);
            }

            if (playerHP <= 0) {
                sheriff.isAlive = 0;
                if (currentScore > highScore) highScore = currentScore;
                gameState = STATE_OVER;
            }
        }
    }

    if (!bandit.isAlive) {
        if (respawnTimer > 0) respawnTimer--;
        else SpawnBandit();
    }

    if (pFlash > 0) pFlash--;
    if (eFlash > 0) eFlash--;

    // Fix dust
    for (int i = 0; i < 20; i++) {
        if (dust[i].life <= 0 || dust[i].x < 0) {
            dust[i].x = (float)(WIN_WIDTH + (rand() % 100));
            dust[i].y = (float)(rand() % (WIN_HEIGHT - 40));
            dust[i].dx = -4.0f - (rand() % 4);
            dust[i].life = 80 + (rand() % 60);
        }
        dust[i].x += dust[i].dx;
        dust[i].life--;
    }

    for (int i = 0; i < 15; i++) {
        if (sparks[i].life > 0) {
            sparks[i].x += sparks[i].dx;
            sparks[i].y += sparks[i].dy;
            sparks[i].life--;
        }
    }
}

// Rendering Functions
void DrawCowboy(HDC hdc, Entity* c, COLORREF shirtCol) {
    if (!c->isAlive) return;

    int cy = c->isDucking ? c->y + 30 : c->y;
    int ch = c->isDucking ? 30 : 60;

    HBRUSH shirtBrush = CreateSolidBrush(shirtCol);
    HBRUSH hatBrush = CreateSolidBrush(COL_HAT);
    HBRUSH jeansBrush = CreateSolidBrush(RGB(20, 20, 60));

    RECT body = { (int)c->x, cy + 15, (int)c->x + c->width, cy + ch - 20 };
    FillRect(hdc, &body, shirtBrush);

    RECT legs = { (int)c->x + 2, cy + ch - 20, (int)c->x + c->width - 2, cy + ch };
    FillRect(hdc, &legs, jeansBrush);

    RECT hatTop = { (int)c->x + 4, cy, (int)c->x + c->width - 4, cy + 10 };
    RECT hatBrim = { (int)c->x - 6, cy + 10, (int)c->x + c->width + 6, cy + 15 };
    FillRect(hdc, &hatTop, hatBrush);
    FillRect(hdc, &hatBrim, hatBrush);

    DeleteObject(shirtBrush);
    DeleteObject(hatBrush);
    DeleteObject(jeansBrush);
}

void RenderScene(HDC hdc) {
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, WIN_WIDTH, WIN_HEIGHT);
    SelectObject(memDC, memBitmap);

    HFONT fontTitle = CreateFontA(56, 0, 0, 0, FW_HEAVY, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Courier New");
    HFONT fontLarge = CreateFontA(36, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Courier New");
    HFONT fontSmall = CreateFontA(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Courier New");
    char textBuf[64];

    if (gameState == STATE_MENU) {
        HBRUSH bgBrush = CreateSolidBrush(COL_UI_BG);
        RECT bg = { 0, 0, WIN_WIDTH, WIN_HEIGHT };
        FillRect(memDC, &bg, bgBrush);
        DeleteObject(bgBrush);

        HBRUSH borderBrush = CreateSolidBrush(COL_UI_ACCENT);
        RECT topBorder = { 20, 20, WIN_WIDTH - 20, 25 };
        RECT botBorder = { 20, WIN_HEIGHT - 25, WIN_WIDTH - 20, WIN_HEIGHT - 20 };
        RECT leftBorder = { 20, 20, 25, WIN_HEIGHT - 20 };
        RECT rightBorder = { WIN_WIDTH - 25, 20, WIN_WIDTH - 20, WIN_HEIGHT - 20 };
        FillRect(memDC, &topBorder, borderBrush);
        FillRect(memDC, &botBorder, borderBrush);
        FillRect(memDC, &leftBorder, borderBrush);
        FillRect(memDC, &rightBorder, borderBrush);
        DeleteObject(borderBrush);

        SetBkMode(memDC, TRANSPARENT);
        SetTextColor(memDC, COL_UI_ACCENT);
        SelectObject(memDC, fontTitle);
        TextOutA(memDC, WIN_WIDTH / 2 - 230, 60, "HIGH NOON DUEL", 14);

        Entity dummy1 = { 220, 150, 0, 24, 60, 0, 0, 1 };
        Entity dummy2 = { 556, 150, 0, 24, 60, 0, 0, 1 };
        DrawCowboy(memDC, &dummy1, COL_SHERIFF);
        DrawCowboy(memDC, &dummy2, COL_BANDIT);

        SelectObject(memDC, fontSmall);
        SetTextColor(memDC, RGB(255, 255, 0));
        sprintf_s(textBuf, sizeof(textBuf), "TOP BOUNTY: $%d", highScore);
        TextOutA(memDC, WIN_WIDTH / 2 - 110, 160, textBuf, (int)strlen(textBuf));

        if ((frameCounter / 30) % 2 == 0) {
            SetTextColor(memDC, RGB(255, 50, 50));
            SelectObject(memDC, fontLarge);
            TextOutA(memDC, WIN_WIDTH / 2 - 130, 250, "INSERT COIN", 11);

            SetTextColor(memDC, RGB(200, 200, 200));
            SelectObject(memDC, fontSmall);
            TextOutA(memDC, WIN_WIDTH / 2 - 165, 300, "[ PRESS ENTER TO PLAY ]", 23);
        }
    }
    else if (gameState == STATE_PLAY || gameState == STATE_OVER) {
        HBRUSH skyBrush = CreateSolidBrush(COL_SKY);
        RECT bg = { 0, 0, WIN_WIDTH, GROUND_Y };
        FillRect(memDC, &bg, skyBrush);
        DeleteObject(skyBrush);

        HBRUSH sunBrush = CreateSolidBrush(COL_SUN);
        SelectObject(memDC, sunBrush);
        SelectObject(memDC, GetStockObject(NULL_PEN));
        Ellipse(memDC, WIN_WIDTH / 2 - 80, GROUND_Y - 120, WIN_WIDTH / 2 + 80, GROUND_Y + 40);
        DeleteObject(sunBrush);

        HBRUSH sandBrush = CreateSolidBrush(COL_SAND);
        RECT ground = { 0, GROUND_Y, WIN_WIDTH, WIN_HEIGHT };
        FillRect(memDC, &ground, sandBrush);
        DeleteObject(sandBrush);

        HBRUSH cactusBrush = CreateSolidBrush(COL_CACTUS);
        RECT cactus1 = { 300, GROUND_Y - 80, 320, GROUND_Y + 10 };
        RECT cactusArm = { 285, GROUND_Y - 50, 300, GROUND_Y - 35 };
        FillRect(memDC, &cactus1, cactusBrush);
        FillRect(memDC, &cactusArm, cactusBrush);
        DeleteObject(cactusBrush);

      //Drawing dust
        HBRUSH dustBrush = CreateSolidBrush(COL_DUST);
        for (int i = 0; i < 20; i++) {
            if (dust[i].life > 0) {
                RECT dRect = { (int)dust[i].x, (int)dust[i].y, (int)dust[i].x + 24, (int)dust[i].y + 3 };
                FillRect(memDC, &dRect, dustBrush);
            }
        }
        DeleteObject(dustBrush);

        DrawCowboy(memDC, &sheriff, COL_SHERIFF);
        DrawCowboy(memDC, &bandit, COL_BANDIT);

        HBRUSH flashBrush = CreateSolidBrush(RGB(255, 255, 200));
        if (pFlash > 0) {
            RECT fRect = { (int)sheriff.x + sheriff.width, (int)sheriff.y + (sheriff.isDucking ? 40 : 20) - 4, (int)sheriff.x + sheriff.width + 12, (int)sheriff.y + (sheriff.isDucking ? 40 : 20) + 4 };
            FillRect(memDC, &fRect, flashBrush);
        }
        if (eFlash > 0) {
            RECT fRect = { (int)bandit.x - 12, (int)bandit.y + (bandit.isDucking ? 40 : 20) - 4, (int)bandit.x, (int)bandit.y + (bandit.isDucking ? 40 : 20) + 4 };
            FillRect(memDC, &fRect, flashBrush);
        }
        DeleteObject(flashBrush);

        HBRUSH bulletBrush = CreateSolidBrush(COL_BULLET);
        if (pBullet.isActive) {
            RECT b = { (int)pBullet.x, (int)pBullet.y, (int)pBullet.x + 10, (int)pBullet.y + 4 };
            FillRect(memDC, &b, bulletBrush);
        }
        if (eBullet.isActive) {
            RECT b = { (int)eBullet.x, (int)eBullet.y, (int)eBullet.x + 10, (int)eBullet.y + 4 };
            FillRect(memDC, &b, bulletBrush);
        }
        DeleteObject(bulletBrush);

        HBRUSH sparkBrush = CreateSolidBrush(RGB(255, 50, 50));
        for (int i = 0; i < 15; i++) {
            if (sparks[i].life > 0) {
                RECT sRect = { (int)sparks[i].x, (int)sparks[i].y, (int)sparks[i].x + 3, (int)sparks[i].y + 3 };
                FillRect(memDC, &sRect, sparkBrush);
            }
        }
        DeleteObject(sparkBrush);

        SetTextColor(memDC, RGB(255, 255, 255));
        SetBkMode(memDC, TRANSPARENT);
        SelectObject(memDC, fontSmall);
        sprintf_s(textBuf, sizeof(textBuf), "BOUNTY: $%d", currentScore);
        TextOutA(memDC, 20, 20, textBuf, (int)strlen(textBuf));

        HBRUSH redBrush = CreateSolidBrush(RGB(255, 40, 40));
        HBRUSH oldBrush = (HBRUSH)SelectObject(memDC, redBrush);
        for (int i = 0; i < playerHP; i++) {
            RECT hpRect = { 20 + (i * 15), 50, 30 + (i * 15), 60 };
            FillRect(memDC, &hpRect, redBrush);
        }
        for (int i = 0; i < enemyHP; i++) {
            RECT hpRect = { WIN_WIDTH - 60 + (i * 15), 50, WIN_WIDTH - 50 + (i * 15), 60 };
            FillRect(memDC, &hpRect, redBrush);
        }
        SelectObject(memDC, oldBrush);
        DeleteObject(redBrush);

        char ammoBuf[32];
        if (currentAmmo == AMMO_NORMAL) sprintf_s(ammoBuf, sizeof(ammoBuf), "AMMO: 1-NORMAL");
        if (currentAmmo == AMMO_FAST)   sprintf_s(ammoBuf, sizeof(ammoBuf), "AMMO: 2-FAST");
        if (currentAmmo == AMMO_HEAVY)  sprintf_s(ammoBuf, sizeof(ammoBuf), "AMMO: 3-HEAVY (X2 DMG)");

        SelectObject(memDC, fontSmall);
        SetTextColor(memDC, COL_UI_ACCENT);
        TextOutA(memDC, WIN_WIDTH / 2 - 100, WIN_HEIGHT - 60, ammoBuf, (int)strlen(ammoBuf));

        if (gameState == STATE_OVER) {
            HBRUSH overlay = CreateSolidBrush(RGB(0, 0, 0));
            RECT ovRect = { WIN_WIDTH / 2 - 160, WIN_HEIGHT / 2 - 60, WIN_WIDTH / 2 + 160, WIN_HEIGHT / 2 + 60 };
            FillRect(memDC, &ovRect, overlay);

            HBRUSH borderBrush = CreateSolidBrush(COL_UI_ACCENT);
            FrameRect(memDC, &ovRect, borderBrush);
            DeleteObject(borderBrush);
            DeleteObject(overlay);

            SetTextColor(memDC, RGB(255, 50, 50));
            SelectObject(memDC, fontLarge);
            TextOutA(memDC, WIN_WIDTH / 2 - 70, WIN_HEIGHT / 2 - 40, "WASTED!", 7);

            SetTextColor(memDC, COL_UI_ACCENT);
            SelectObject(memDC, fontSmall);
            TextOutA(memDC, WIN_WIDTH / 2 - 135, WIN_HEIGHT / 2 + 10, "PRESS ENTER TO MENU", 19);
        }
    }

    HPEN scanPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    SelectObject(memDC, scanPen);
    for (int y = 0; y < WIN_HEIGHT; y += 4) {
        MoveToEx(memDC, 0, y, NULL);
        LineTo(memDC, WIN_WIDTH, y);
    }
    DeleteObject(scanPen);

    BitBlt(hdc, 0, 0, WIN_WIDTH, WIN_HEIGHT, memDC, 0, 0, SRCCOPY);

    DeleteObject(fontTitle); DeleteObject(fontLarge); DeleteObject(fontSmall);
    DeleteObject(memBitmap); DeleteDC(memDC);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_DESTROY) { PostQuitMessage(0); return 0; }
    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSA wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "WildWestArcade";

    RegisterClassA(&wc);

    HWND hwnd = CreateWindowExA(0, "WildWestArcade", "HIGH NOON DUEL",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    srand((unsigned)time(NULL));

    MSG msg;
    int enterPressed = 0;

    while (1) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        frameCounter++;
        int enterState = GetAsyncKeyState(VK_RETURN) & 0x8000;

        switch (gameState) {
        case STATE_MENU:
            if (enterState && !enterPressed) {
                ResetGameSession();
                gameState = STATE_PLAY;
            }
            break;

        case STATE_PLAY:
            sheriff.isDucking = (GetAsyncKeyState(VK_DOWN) & 0x8000) && !sheriff.isJumping;

            if ((GetAsyncKeyState(VK_UP) & 0x8000) && !sheriff.isJumping && !sheriff.isDucking) {
                sheriff.isJumping = 1;
                sheriff.dy = -15.0f;
            }

            if ((GetAsyncKeyState(VK_SPACE) & 0x8000) && !pBullet.isActive) {
                pBullet.x = sheriff.x + sheriff.width;
                pBullet.y = sheriff.isDucking ? sheriff.y + 40 : sheriff.y + 20;

                if (currentAmmo == AMMO_FAST)  pBullet.dx = 25.0f;
                else if (currentAmmo == AMMO_HEAVY) pBullet.dx = 8.0f;
                else pBullet.dx = 15.0f;

                pBullet.isActive = 1;
                pFlash = 4;
            }
            UpdatePhysicsAndLogic();
            break;

        case STATE_OVER:
            if (enterState && !enterPressed) {
                gameState = STATE_MENU;
            }
            break;
        }

        enterPressed = enterState;

        HDC hdc = GetDC(hwnd);
        RenderScene(hdc);
        ReleaseDC(hwnd, hdc);

        Sleep(FPS_DELAY);
    }
    return (int)msg.wParam;
}