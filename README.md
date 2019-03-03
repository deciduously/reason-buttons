# Reason Buttons

Simple `bs-socket` demo application with ReasonReact client app.   Companion repo for [Real-Time Communication in ReasonML with bs-socket](https://dev.to/deciduously/real-time-communication-in-reasonml-with-bs-socket-1p5l).

## Dependencies

* [Yarn](https://yarnpkg.com/en/)

## Usage

First, run `yarn install` in the project root.  Then open three terminals.  In one, run `yarn start:re` to start the Bucklescript compiler, in another run `yarn start:bundle` to run [Parcel](https://parceljs.org/), and finally `yarn serve` to start the server.  Then open a few browser tabs to `localhost:3000` and click away.  Port can also be specified by the `PORT` environment variable but defaults to 3000.