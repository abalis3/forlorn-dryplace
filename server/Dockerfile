FROM alpine:3.12 AS builder

RUN apk add --no-cache build-base protobuf-dev
RUN mkdir /fd-server
WORKDIR /fd-server
COPY ./server/src ./src
COPY ./shared-src ./src

# Execute the actual build steps
RUN mkdir bin
RUN if [ -d "src/pbuf/generated" ]; then rm -Rf src/pbuf/generated; fi
RUN mkdir src/pbuf/generated
RUN protoc -I ./src/pbuf --cpp_out=./src/pbuf/generated ./src/pbuf/*.proto
RUN g++ -I ./src ./src/*.cpp ./src/pbuf/generated/*.cc -lprotobuf -o ./bin/fd-server

FROM alpine:3.12 as prod-img

COPY --from=builder /fd-server/bin/fd-server /
RUN apk add --no-cache libstdc++ protobuf
EXPOSE 44444
CMD ["/fd-server"]