module Client = BsSocket.Client.Make(Messages);

let socket = Client.create();

type state = {
  numClients: int,
  buttons: list(string),
  newButtonTitle: string,
};

type action =
  | AddButton(string)
  | ClientDelta(int)
  | RemoveButton(string)
  | Success((int, list(string)))
  | UpdateTitle(string);

let component = ReasonReact.reducerComponent("ButtonClient");

let make = _children => {
  ...component,
  initialState: _state => {numClients: 1, buttons: [], newButtonTitle: ""},
  didMount: self => {
    Client.on(socket, m =>
      switch (m) {
      | Msg(msg) =>
        switch (msg) {
        | AddButton(name) => self.send(AddButton(name))
        | RemoveButton(name) => self.send(RemoveButton(name))
        }
      | ClientDelta(amt) => self.send(ClientDelta(amt))
      | Success((numClients, buttons)) =>
        self.send(Success((numClients, buttons)))
      }
    );
    Client.emit(socket, Howdy);
  },
  reducer: (action, state) =>
    switch (action) {
    | AddButton(name) =>
      ReasonReact.Update({
        ...state,
        buttons: List.append(state.buttons, [name]),
      })
    | ClientDelta(amt) =>
      ReasonReact.Update({...state, numClients: state.numClients + amt})
    | RemoveButton(name) =>
      ReasonReact.Update({
        ...state,
        buttons: List.filter(b => b != name, state.buttons),
      })
    | Success((numClients, buttons)) =>
      ReasonReact.Update({...state, numClients, buttons})
    | UpdateTitle(newButtonTitle) =>
      ReasonReact.Update({...state, newButtonTitle})
    },
  render: self =>
    <div>
      <h1> {ReasonReact.string("Reason Buttons")} </h1>
      <div>
        {ReasonReact.array(
           self.state.buttons
           |> List.map(button =>
                <button
                  key=button
                  onClick={_ => {
                    self.send(RemoveButton(button));
                    Client.emit(socket, Msg(RemoveButton(button)));
                  }}>
                  {ReasonReact.string(button)}
                </button>
              )
           |> Array.of_list,
         )}
      </div>
      <div>
        <input
          type_="text"
          value={self.state.newButtonTitle}
          onChange={evt =>
            self.send(UpdateTitle(ReactEvent.Form.target(evt)##value))
          }
        />
        <button
          onClick={_ => {
            let name = self.state.newButtonTitle;
            self.send(UpdateTitle(""));
            self.send(AddButton(name));
            Client.emit(socket, Msg(AddButton(name)));
          }}>
          {ReasonReact.string("Add button " ++ self.state.newButtonTitle)}
        </button>
      </div>
      <span>
        {ReasonReact.string(
           (self.state.numClients |> string_of_int) ++ " connected",
         )}
      </span>
    </div>,
};