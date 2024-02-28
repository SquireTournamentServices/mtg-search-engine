FROM ubuntu:latest as base
ARG DEBIAN_FRONTEND=noninteractive

ENV TZ=Europe/London
RUN apt-get update
RUN apt-get install -y libjansson-dev libjemalloc-dev libabsl-dev libmbedtls-dev libssl-dev build-essential cmake gcovr valgrind python3 python3-pip bison flex libcurl4-openssl-dev

FROM base as build
RUN useradd app

RUN mkdir /app
RUN chown -R app /app
RUN chgrp -R app /app

USER app
WORKDIR /app

COPY . .
USER root
RUN pip3 install -r requirements.txt
USER app

RUN mkdir build
WORKDIR /app/build

RUN cmake -DMSE_WEB_API=ON -DUSE_JEMALLOC=ON ..
RUN cmake --build . -j

FROM base as app
WORKDIR /app
COPY --from=build /app/build/mtg-search-engine-web-api .
COPY --from=build /app/build/AllPrintings.json .

# Force loading of bundled JSON file
ENV MSE_TEST 1
EXPOSE 4365
CMD ./mtg-search-engine-web-api
