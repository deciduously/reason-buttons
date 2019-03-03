open Belt.Option;
open Express;
open Extern;

let port = getWithDefault(portEnv |> Js.Nullable.toOption, "3000");

let app = Express.express();

let http = Http.create(app);

App.use(
  app,
  {
    let options = Static.defaultOptions();
    Static.make(Path.join([|__dirname, "../dist"|]), options)
    |> Static.asMiddleware;
  },
);

App.get(app, ~path="/") @@
Middleware.from((_, _, res) =>
  res |> Response.sendFile("index.html", {"root": __dirname})
);

module Server = BsSocket.Server.Make(Messages);

type appState = {
  buttons: list(string),
  clients: list(BsSocket.Server.socketT),
};

let state = ref({buttons: ["Click me"], clients: []});

let io = Server.createWithHttp(http);

let sendToRest = (socket, msg) =>
  state^.clients
  |> List.filter(c => c != socket)
  |> List.iter(c => Server.Socket.emit(c, msg));

Server.onConnect(
  io,
  socket => {
    open Server;
    print_endline("Client connected");
    state := {...state^, clients: List.append(state^.clients, [socket])};
    sendToRest(socket, ClientDelta(1));
    Socket.emit(
      socket,
      Success((List.length(state^.clients), state^.buttons)),
    );
    Socket.onDisconnect(
      socket,
      _ => {
        print_endline("Client disconnected");
        sendToRest(socket, ClientDelta(-1));
        state :=
          {...state^, clients: List.filter(c => c == socket, state^.clients)};
      },
    );
    Socket.on(
      socket,
      fun
      | Msg(msg) => {
          switch (msg) {
          | AddButton(name) =>
            print_endline("Add " ++ name);
            state :=
              {...state^, buttons: state^.buttons |> List.append([name])};
            sendToRest(socket, Msg(AddButton(name)));
          | RemoveButton(name) =>
            print_endline("Remove " ++ name);
            state :=
              {
                ...state^,
                buttons: state^.buttons |> List.filter(a => a == name),
              };
            sendToRest(socket, Msg(RemoveButton(name)));
          };
        }
      | Howdy => {
          print_endline("Howdy back, client");
        },
    );
  },
);

Http.listen(http, port |> int_of_string, () =>
  print_endline("Listening at *:" ++ port)
);