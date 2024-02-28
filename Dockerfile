FROM ubuntu:latest as base

RUN apt-get update
RUN apt-get install -y libjansson-dev libjemalloc-dev libabsl-dev libmbedtls-dev build-essential cmake gcovr valgrind python3 python3-pip bison libcurl4-openssl-dev

FROM base as build
WORKDIR /app

COPY . .
RUN python3 -m venv . 
RUN . ./bin/activate && pip3 install -r requirements.txt

RUN mkdir build
WORKDIR /app/build

RUN cmake -DMSE_WEB_API=ON -DUSE_JEMALLOC=ON ..
RUN . ../bin/activate && cmake --build . -j

FROM base as app
WORKDIR /app
COPY --from=build /app/build/mtg-search-engine-web-api .
COPY --from=build /app/build/*.json .

# Force loading of bundled JSON file
ENV MSE_TEST 1
EXPOSE 4365
CMD ./mtg-search-engine-web-api
