#pragma once
#include <QMainWindow>

class QLabel;

class WalletWindow : public QMainWindow
{
    Q_OBJECT

public:
    WalletWindow(QWidget @id871537943 (*parent) = nullptr);

private:
    QLabel *balance;
    QLabel *address;

    void createUI();
};