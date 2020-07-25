FROM alpine:3.12 AS builder

RUN apk add --no-cache build-base
RUN mkdir /fd-server
WORKDIR /fd-server
COPY ./src ./src

RUN mkdir bin
RUN g++ -I ./src ./src/*.cpp -o ./bin/fd-server

FROM alpine:3.12 as prod-img

COPY --from=builder /fd-server/bin/fd-server /
RUN apk add --no-cache libstdc++
EXPOSE 44444
CMD ["/fd-server"]