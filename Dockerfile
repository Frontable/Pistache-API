FROM ubuntu:24.04 AS build

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        pkg-config \
        libpistache-dev \
        libpq-dev \
        nlohmann-json3-dev \
        libssl-dev \
        ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .


RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF \
    && cmake --build build -j"$(nproc)"

FROM ubuntu:24.04

RUN apt-get update && apt-get install -y --no-install-recommends \
        libpistache0t64 \
        libpq5 \
        libssl3 \
        ca-certificates \
    && rm -rf /var/lib/apt/lists/* \
    && useradd --system --create-home --shell /usr/sbin/nologin appuser

WORKDIR /app
COPY --from=build /src/build/pistache_api ./pistache_api
COPY --from=build /src/frontend ./frontend

ENV FRONTEND_DIR=/app/frontend
ENV PORT=9080

USER appuser
EXPOSE 9080

CMD ["./pistache_api"]
