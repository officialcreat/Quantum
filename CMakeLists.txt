cmake_minimum_required(VERSION 3.20)

project(MyCoin VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Qt6 REQUIRED COMPONENTS Widgets Svg)
find_package(OpenSSL REQUIRED)

include_directories(src)

qt_add_executable(QuantumWallet
    src/main.cpp
    src/wallet_gui.cpp

    src/crypto.cpp
    src/keypair.cpp
    src/signature.cpp
    src/base58.cpp
    src/script.cpp
    src/transaction.cpp
    src/block.cpp
    src/blockchain.cpp
    src/storage.cpp
    src/wallet.cpp
    src/p2p.cpp
    src/rpc.cpp
    src/node.cpp
)

target_link_libraries(QuantumWallet PRIVATE
    Qt6::Widgets
    Qt6::Svg
    OpenSSL::Crypto
    OpenSSL::SSL
    ws2_32
)
