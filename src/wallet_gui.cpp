#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <rpc.h>
#include <windows.h>
#include <cstring>
#include <cstdlib>

#include "blockchain.h"
#include "storage.h"
#include "wallet.h"

// Цвета Quantum
COLORREF BG = RGB(11, 16, 32);
COLORREF CARD = RGB(21, 27, 49);
COLORREF PURPLE = RGB(124, 58, 237);

HBRUSH bgBrush = CreateSolidBrush(BG);
HBRUSH cardBrush = CreateSolidBrush(CARD);

Blockchain chain(nullptr);
Wallet wallet1(5000);
Wallet wallet2(5001);

HWND txtAddress;
HWND txtAmount;
HWND lblBalance;
HWND lblBalance2;
HWND lstBlocks;
HWND lstHistory;

void UpdateBalance()
{
    std::string s1 =
        "Balance1: " +
        std::to_string((int)chain.getBalance(wallet1.getAddress())) +
        " QTM";

    SetWindowTextA(lblBalance, s1.c_str());

    std::string s2 =
        "Balance2: " +
        std::to_string((int)chain.getBalance(wallet2.getAddress())) +
        " QTM";

    SetWindowTextA(lblBalance2, s2.c_str());
}

void UpdateHistory()
{
    SendMessageA(lstHistory, LB_RESETCONTENT, 0, 0);

    const auto& blocks = chain.getChain();

    for (const auto& block : blocks)
    {
        for (const auto& tx : block.transactions)
        {
            std::string s =
                tx.from + " -> " +
                tx.to + " : " +
                std::to_string((int)tx.amount);

            SendMessageA(
                lstHistory,
                LB_ADDSTRING,
                0,
                (LPARAM)s.c_str()
            );
        }
    }
}

LRESULT CALLBACK WndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (msg)
    {
    case WM_ERASEBKGND:
    {
        RECT rc;
        GetClientRect(hwnd, &rc);

        FillRect((HDC)wParam, &rc, bgBrush);

        RECT card = {15, 45, 355, 120};
        FillRect((HDC)wParam, &card, cardBrush);

        return 1;
    }

    case WM_CTLCOLORSTATIC:
    {
        HDC hdc = (HDC)wParam;

        SetTextColor(hdc, RGB(255, 255, 255));
        SetBkColor(hdc, BG);

        return (INT_PTR)bgBrush;
    }

    case WM_CREATE:
    {
        // Заголовок
        CreateWindowA(
            "STATIC",
            "QUANTUM WALLET",
            WS_CHILD | WS_VISIBLE,
            25, 20, 300, 30,
            hwnd,
            NULL,
            NULL,
            NULL
        );

        // Баланс
        lblBalance = CreateWindowA(
            "STATIC",
            "350.000 QTM",
            WS_CHILD | WS_VISIBLE,
            25, 25, 260, 24,
            hwnd,
            NULL,
            NULL,
            NULL
        );

        // Адрес
        CreateWindowA(
            "STATIC",
            "Wallet Address",
            WS_CHILD | WS_VISIBLE,
            25, 95, 320, 20,
            hwnd,
            NULL,
            NULL,
            NULL
        );

        txtAddress = CreateWindowA(
            "EDIT",
            "",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            25, 120, 320, 26,
            hwnd,
            (HMENU)2,
            NULL,
            NULL
        );

        // Сумма
        txtAmount = CreateWindowA(
            "EDIT",
            "",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            25, 160, 140, 26,
            hwnd,
            (HMENU)3,
            NULL,
            NULL
        );

        // Кнопка SEND
        CreateWindowA(
            "BUTTON",
            "SEND",
            WS_CHILD | WS_VISIBLE,
            185, 160, 160, 34,
            hwnd,
            (HMENU)4,
            NULL,
            NULL
        );

        // Кнопка MINE
        CreateWindowA(
            "BUTTON",
            "MINE",
            WS_CHILD | WS_VISIBLE,
            25, 205, 320, 34,
            hwnd,
            (HMENU)1,
            NULL,
            NULL
        );

        // История
        lstHistory = CreateWindowA(
            "LISTBOX",
            "",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            25, 255, 320, 120,
            hwnd,
            NULL,
            NULL,
            NULL
        );

        Storage::Load(chain, "blockchain.dat");

        if (chain.getChain().empty())
        {
            chain.createGenesisBlock();
            Storage::Save(chain, "blockchain.dat");
        }

        // Блоки
        lstBlocks = CreateWindowA(
            "LISTBOX",
            "",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            25, 390, 320, 110,
            hwnd,
            NULL,
            NULL,
            NULL
        );

        UpdateBalance();
        UpdateHistory();

        Storage::Save(chain, "blockchain.dat");

        break;
    }

    case WM_COMMAND:
    {
        if (LOWORD(wParam) == 1)
        {
            chain.mineBlock(wallet1.getAddress());

            UpdateBalance();
            UpdateHistory();

            Storage::Save(chain, "blockchain.dat");
        }
        else if (LOWORD(wParam) == 4)
        {
            char addr[128];
            char amountStr[32];

            GetWindowTextA(txtAddress, addr, 128);
            GetWindowTextA(txtAmount, amountStr, 32);

            if (strlen(addr) == 0)
                strcpy(addr, wallet2.getAddress().c_str());

            double amount = atof(amountStr);

            if (chain.send(wallet1, std::string(addr), amount))
            {
                UpdateBalance();
                UpdateHistory();

                Storage::Save(chain, "blockchain.dat");

                SetWindowTextA(txtAddress, "");
                SetWindowTextA(txtAmount, "");
            }
            else
            {
                MessageBoxA(
                    hwnd,
                    "Not enough balance or invalid address!",
                    "Transaction error",
                    MB_OK | MB_ICONERROR
                );
            }
        }

        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(
    HINSTANCE h,
    HINSTANCE,
    LPSTR,
    int)
{
    WNDCLASSA wc = {};

    wc.lpfnWndProc = WndProc;
    wc.hInstance = h;
    wc.lpszClassName = "Quantum";

    wc.hIcon = (HICON)LoadImageA(
        NULL,
        "assets\\quantum.ico",
        IMAGE_ICON,
        256,
        256,
        LR_LOADFROMFILE
    );

    RegisterClassA(&wc);

    HWND hwnd = CreateWindowA(
        "Quantum",
        "Quantum Wallet",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        200,
        150,
        420,
        560,
        NULL,
        NULL,
        h,
        NULL
    );

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}