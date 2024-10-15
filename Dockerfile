FROM ubuntu:latest as base
ARG DEBIAN_FRONTEND=noninteractive

ENV TZ=Europe/London
RUN apt-get update
RUN apt-get install -y libjansson-dev libjemalloc-dev libabsl-dev libmbedtls-dev libssl-dev build-essential cmake gcovr valgrind python3 python3-pip bison flex libcurl4-openssl-dev

FROM base AS build
RUN useradd app

RUN mkdir /app
RUN chmod -R 755 /app
RUN chown -R app /app
RUN chgrp -R app /app

USER app
WORKDIR /app

COPY . .
USER root
RUN pip3 install -r requirements.txt --break-system-packages
USER app

RUN mkdir build
WORKDIR /app/build

RUN cmake -DMSE_WEB_API=ON -DUSE_JEMALLOC=ON ..
RUN cmake --build . -j

FROM base AS app
WORKDIR /app
COPY --from=build /app/build/mtg-search-engine-web-api .
# Bundle tests, to allow for sanity checking the image
COPY --from=build /app/build/mtg-search-engine-tests .

# Force caching via test flag
ENV MSE_TEST=true
COPY --from=build /app/build/AllPrintings.json .

EXPOSE 4365
CMD ./mtg-search-engine-web-api
