FROM ubuntu:latest AS base
ARG DEBIAN_FRONTEND=noninteractive
ARG DEBUG=OFF
ARG ADDRESS_SANITISER=OFF

ENV TZ=Europe/London
RUN apt-get update && apt-get install -y libjansson-dev libjemalloc-dev libabsl-dev libmbedtls-dev libssl-dev build-essential cmake gcovr python3 python3-pip libcurl4-openssl-dev

FROM base AS build
RUN useradd app

RUN mkdir /app
RUN chmod -R 755 /app
RUN chown -R app /app
RUN chgrp -R app /app

USER app
WORKDIR /app

COPY ./requirements.txt .
USER root
RUN pip3 install -r requirements.txt --break-system-packages
USER app

COPY . .

RUN mkdir build
WORKDIR /app/build

ENV MSE_TEST=true
# Forces the final image size to be smaller
RUN cmake -DUSE_JEMALLOC=ON -DDEBUG=$DEBUG -DADDRESS_SANITISER=$ADDRESS_SANITISER .. && \
  cmake --build . -j && \
  ./mtg-search-engine-cli --generate && \
  cp mtg-search-engine-tests .. && \
  cp mtg-search-engine-web-api .. && \
  cp cards.data .. && \
  cd .. && \
  rm -rf build

FROM base AS app
WORKDIR /app
COPY --from=build /app/mtg-search-engine-web-api .
# Bundle tests, to allow for sanity checking the image
COPY --from=build /app/mtg-search-engine-tests .

# Copy in data file for startup
COPY --from=build /app/cards.data .

EXPOSE 4365
CMD ./mtg-search-engine-web-api
