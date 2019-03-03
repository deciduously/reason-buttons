[@bs.val] external portEnv: Js.Nullable.t(string) = "process.env.PORT";
[@bs.val] external parseInt: (string, int) => int = "parseInt";

module Http = {
  type http;
  [@bs.module "http"] external create: Express.App.t => http = "Server";
  [@bs.send] external listen: (http, int, unit => unit) => unit = "";
};

// https://github.com/reasonml-community/bs-socket.io/blob/master/example/ExampleServer.re

module Path = {
  type pathT;
  [@bs.module "path"] [@bs.splice]
  external join: array(string) => string = "";
};

[@bs.val] external __dirname: string = "";