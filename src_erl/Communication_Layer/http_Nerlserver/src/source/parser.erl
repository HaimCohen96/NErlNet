
%%%-------------------------------------------------------------------
%%% @author kapelnik
%%% @copyright (C) 2021, <COMPANY>
%%% @doc
%%%
%%% @end
%%% Created : 19. Apr 2021 4:27 AM
%%%-------------------------------------------------------------------
-module(parser).
-author("kapelnik").

%% API
-export([parse/2]).



%%use this decoder to decode one line after parsing
%%    decodeList(Binary)->  decodeList(Binary,[]).
%%    decodeList(<<>>,L) -> L;
%%    decodeList(<<A:64/float,Rest/binary>>,L) -> decodeList(Rest,L++[A]).

%%this parser takes a CSV folder containing chunked data, parsing into a list of binary.
%%each record in the line is a batch of samples
parse(ChunkSize,FolderName)->
  io:format("curr dir: ~p~n",[file:get_cwd()]),
%%  FolderName="./input/shuffled-input1_splitted/",
  parse_all(ChunkSize,FolderName,1,[]).


parse_all(ChunkSize,FolderName,Counter,Ret)->
  Name = lists:last(re:split(FolderName,"/",[{return,list}])),
  try   parse_file(ChunkSize,"../../../inputDataDir/"++FolderName++"_splitted/"++Name++integer_to_list(Counter)++".csv") of
    L ->
      parse_all(ChunkSize,FolderName,Counter+1,Ret++L)
  catch
    error: E->io:format("#####Error at Parser: ~n~p~n",[E]),Ret
  end.

%%parsing a given CSV file
parse_file(ChunkSize,File_Address) ->

    io:format("File_Address:~p~n~n",[File_Address]),

  {ok, Data} = file:read_file(File_Address),%%TODO change to File_Address
  Lines = re:split(Data, "\r|\n|\r\n", [{return,binary}] ),

  SampleSize = length(re:split(binary_to_list(hd(Lines)), ",", [{return,list}])),
%%  get binary lines
  ListsOfListsOfFloats = encodeListOfLists(Lines),

%%chunk data
  Chunked= makeChunks(ListsOfListsOfFloats,ChunkSize,ChunkSize,<<>>,[],SampleSize),
%%  io:format("Chunked!~n",[]),
%%%%  Decoded = decodeListOfLists(Chunked ),
%%
%%  io:format("Decoded!!!: ~n",[]),
  Chunked.

encodeListOfLists(L)->encodeListOfLists(L,[]).
encodeListOfLists([],Ret)->
  Ret;
encodeListOfLists([[<<>>]|Tail],Ret)->
  encodeListOfLists(Tail,Ret);
encodeListOfLists([Head|Tail],Ret)->
  encodeListOfLists(Tail,Ret++[encodeFloatsList(Head)]).


%%return a binary representing a list of floats: List-> <<binaryofthisList>>
encodeFloatsList(L)->
  Splitted = re:split(binary_to_list(L), ",", [{return,list}]),
  encodeFloatsList(Splitted,<<>>).
encodeFloatsList([],Ret)->Ret;
encodeFloatsList([<<>>|ListOfFloats],Ret)->
  encodeFloatsList(ListOfFloats,Ret);
encodeFloatsList([[]|ListOfFloats],Ret)->
  encodeFloatsList(ListOfFloats,Ret);
encodeFloatsList([H|ListOfFloats],Ret)->
    try list_to_float(H) of
    Float->
      encodeFloatsList(ListOfFloats,<<Ret/binary,Float:64/float>>)
  catch
    error:_Error->
      Integer = list_to_integer(H),
      encodeFloatsList(ListOfFloats,<<Ret/binary,Integer:64/float>>)

  end.





makeChunks(L,1,1,_,_,_SampleSize) ->L;
makeChunks([],_Left,_ChunkSize,Acc,Ret,_SampleSize) ->
  Ret++[Acc];

makeChunks([Head|Tail],1,ChunkSize,Acc,Ret,SampleSize) ->
  makeChunks(Tail,ChunkSize,ChunkSize,<<>>,Ret++[<<ChunkSize:64/float,SampleSize:64/float,1:64/float,Acc/binary,Head/binary>>],SampleSize);

makeChunks([Head|Tail],Left,ChunkSize,Acc,Ret,SampleSize) ->
  makeChunks(Tail,Left-1,ChunkSize,<<Acc/binary,Head/binary>>,Ret,SampleSize).
