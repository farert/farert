FROM ubuntu:24.04

# 基本的なパッケージをインストール
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    g++ \
    make \
    cmake \
    gdb \
    gcov \
    lcov \
    git \
    curl \
    wget \
    vim \
    nano \
    tree \
    htop \
    sqlite3 \
    libsqlite3-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# デバッグ情報とカバレッジ情報を含めたコンパイラ設定
ENV CC=gcc
ENV CXX=g++
ENV CFLAGS="-g -fprofile-arcs -ftest-coverage"
ENV CXXFLAGS="-g -fprofile-arcs -ftest-coverage"
ENV LDFLAGS="-lgcov"

# ワーキングディレクトリを設定
WORKDIR /workspace

# ユーザーを作成（VSCodeとの権限問題を避けるため）
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID

RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    && apt-get update \
    && apt-get install -y sudo \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

USER $USERNAME