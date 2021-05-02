%%%-------------------------------------------------------------------
%%% @author kapelnik
%%% @copyright (C) 2021, <COMPANY>
%%% @doc
%%%
%%% @end
%%% Created : 02. Jan 2021 4:05 AM
%%%-------------------------------------------------------------------
-module(casting_handler).
-author("kapelnik").
-export([init/2,  start/2, stop/1]).
-behaviour(application).


%%setter handler for editing weights in CSV file, can also send a reply to sender
init(Req0, [Client_StateM_Pid,Action]) ->
  %Bindings also can be accesed as once, giving a map of all bindings of Req0:
  {_,Body,_} = cowboy_req:read_body(Req0),
  io:format("casting handler got Body:~p~n",[Body]),
  case Action of
    start ->  gen_statem:cast(Client_StateM_Pid, {start_casting,Body});
    stop ->  gen_statem:cast(Client_StateM_Pid, {stop_casting,Body})
  end,
  Reply = io_lib:format("Body Received: ~p~n ", [Body]),
  Req = cowboy_req:reply(200,
    #{<<"content-type">> => <<"text/plain">>},
    Reply,
    Req0),
  {ok, Req, Client_StateM_Pid}.


start(StartType, StartArgs) ->
  erlang:error(not_implemented).

stop(State) ->
  erlang:error(not_implemented).