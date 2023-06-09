-module(nerlNIF).
-include_lib("kernel/include/logger.hrl").
-include("nerlTensor.hrl").

-export([init/0,create_nif/6,train_nif/6,call_to_train/6,predict_nif/3,call_to_predict/6,get_weights_nif/1,printTensor/2]).
-export([call_to_get_weights/1,call_to_set_weights/2]).
-export([decode_nif/2, nerltensor_binary_decode/2]).
-export([encode_nif/2, nerltensor_encode/5, nerltensor_conversion/2, get_all_binary_types/0, get_all_nerltensor_list_types/0]).
-export([erl_type_conversion/1]).

-import(nerl,[even/1, odd/1]).

-on_load(init/0).

% math of nerltensors
-export([nerltensor_sum_nif/3, nerltensor_sum_erl/2]).
-export([nerltensor_scalar_multiplication_nif/3, nerltensor_scalar_multiplication_erl/2]).
-export([sum_nerltensors_lists/2, sum_nerltensors_lists_erl/2]).

init() ->
      NELNET_LIB_PATH = ?NERLNET_PATH++?BUILD_TYPE_RELEASE++"/"++?NERLNET_LIB,
      RES = erlang:load_nif(NELNET_LIB_PATH, 0),
      RES.


% ModelID - Unique ID of the neural network model 
% ModelType - E.g. Regression, Classification 
create_nif(_ModelID, _ModelType , _ScalingMethod , _LayerTypesList , _LayersSizes , _LayersActivationFunctions) ->
      exit(nif_library_not_loaded).

train_nif(_ModelID,_OptimizationMethod,_LossMethod, _LearningRate,_DataTensor,_Type) ->
      exit(nif_library_not_loaded).

call_to_train(ModelID,OptimizationMethod,LossMethod,LearningRate, {DataTensor, Type}, WorkerPid)->
      % io:format("before train  ~n "),
       %io:format("DataTensor= ~p~n ",[DataTensor]),
       %{FakeTensor, Type} = nerltensor_conversion({[2.0,4.0,1.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0], erl_float}, float),
      _RetVal=train_nif(ModelID,OptimizationMethod,LossMethod,LearningRate, DataTensor, Type),
      %io:format("Train Time= ~p~n ",[RetVal]),
      receive
            Ret->
                  % io:format("Ret= ~p~n ",[Ret]),
                  %io:format("WorkerPid,{loss, Ret}: ~p , ~p ~n ",[WorkerPid,{loss, Ret}]),
                  gen_statem:cast(WorkerPid,{loss, Ret}) % TODO @Haran - please check what worker does with this Ret value 
            after ?TRAIN_TIMEOUT ->  %TODO inspect this timeout 
                  ?LOG_ERROR("Worker train timeout reached! setting loss = -1~n "),
                  gen_statem:cast(WorkerPid,{loss, -1.0})
      end.

call_to_predict(ModelID, BatchTensor, Type, WorkerPid,CSVname, BatchID)->
      % io:format("satrting pred_nif~n"),
      _RetVal = predict_nif(ModelID, BatchTensor, Type),
      receive
            
            [PredNerlTensor, NewType, TimeTook]->
                  % io:format("pred_nif done~n"),
                  % {PredTen, _NewType} = nerltensor_conversion({PredNerlTensor, NewType}, erl_float),
                  % io:format("Pred returned: ~p~n", [PredNerlTensor]),
                  gen_statem:cast(WorkerPid,{predictRes,PredNerlTensor, NewType, TimeTook,CSVname, BatchID});
            Error ->
                  ?LOG_ERROR("received wrong prediction_nif format:"++Error),
                  throw("received wrong prediction_nif format")
            after ?PREDICT_TIMEOUT -> 
                 % worker miss predict batch  TODO - inspect this code
                  ?LOG_ERROR("Worker prediction timeout reached! ~n "),
                  gen_statem:cast(WorkerPid,{predictRes, nan, CSVname, BatchID})
      end.

call_to_get_weights(ModelID)->
      try
            _RetVal = get_weights_nif(ModelID),
            receive
                  NerlTensorWeights -> %% NerlTensor is tuple: {Tensor, Type}
                        % io:format("Got Weights= ~p~n",[NerlTensorWeights]),
                        % WorkerPID ! {myWeights, Weights}
                        NerlTensorWeights
            end
      catch Err:E -> ?LOG_ERROR("Couldnt get weights from worker~n~p~n",{Err,E}),
            []
      end.

call_to_set_weights(ModelID,{WeightsNerlTensor, Type})->
      _RetVal = set_weights_nif(ModelID, WeightsNerlTensor, Type).

predict_nif(_ModelID, _BatchTensor, _Type) ->
      exit(nif_library_not_loaded).

get_weights_nif(_ModelID) ->
      exit(nif_library_not_loaded).

set_weights_nif(_ModelID, _Weights, _Type) ->
      exit(nif_library_not_loaded).

printTensor(List,_Type) when is_list(List) -> 
      exit(nif_library_not_loaded).


nerltensor_encode(X,Y,Z,List,Type) when is_number(X) and is_number(Y) and
                                        is_number(Z) and is_list(List) and is_atom(Type)-> 
      case Type of
            erl_float -> {[X,Y,Z] ++ List, erl_float}; % Make sure list of float
            erl_int -> {[X,Y,Z] ++ List, erl_int}; % make sure list of integers
            _COMPRESSED_TYPE -> encode_nif([X,Y,Z] ++ List, Type) % returns {Binary, Type}
      end.

% Input: List and the type of the encoded binary (atom from the group ?BINARY_GROUP_NERLTENSOR_TYPE)
% Output: {Binary,BinaryType}
% Warning - if _XYZ_LIST_FORM type is double it can be cast to integer if binaryType is an integer
encode_nif(_XYZ_LIST_FORM, _BinaryType)  when erlang:is_list(_XYZ_LIST_FORM) and erlang:is_atom(_BinaryType) ->
      exit(nif_library_not_loaded). 

% Input: Binary and Binary Type (atom from the group ?BINARY_GROUP_NERLTENSOR_TYPE)
% Output: {List, ListType} (ListType is an atom from the group ?LIST_GROUP_NERLTENSOR_TYPE)
decode_nif(_Binary, _BinaryType) when erlang:is_binary(_Binary) and erlang:is_atom(_BinaryType) ->
      exit(nif_library_not_loaded). % returns {List,ListType}

% Only float/double types are supported
nerltensor_sum_nif(_BinaryA, _BinaryB, _Mutual_Binary_Type) -> 
      exit(nif_library_not_loaded). % returns {Binary, Type}

% Only float/double types are supported
nerltensor_scalar_multiplication_nif(_NerlTensorBinary, _BinaryType, _ScalarValue) -> 
      exit(nif_library_not_loaded). % returns {Binary, Type}

%---------- nerlTensor -----------%
nerltensor_binary_decode(Binary, Type) when erlang:is_binary(Binary) and erlang:is_atom(Type) ->
      NerlTensorListForm = decode_nif(Binary, Type),
      NerlTensorListForm.

% return the merged list of all supported binary types
get_all_binary_types() -> ?LIST_BINARY_FLOAT_NERLTENSOR_TYPE ++ ?LIST_BINARY_INT_NERLTENSOR_TYPE.
get_all_nerltensor_list_types() -> ?LIST_GROUP_NERLTENSOR_TYPE.
% nerltensor_conversion:
% Type is Binary then: Binary (Compressed Form) --> Erlang List
% Type is list then: Erlang List --> Binary
nerltensor_conversion({NerlTensor, Type}, ResType) ->
      TypeListGroup = lists:member(Type, get_all_nerltensor_list_types()),
      ResTypeListGroup = lists:member(ResType, get_all_nerltensor_list_types()),

      {Operation, ErlType, BinType} = 
                  case {TypeListGroup, ResTypeListGroup} of 
                  {true, false} -> {encode, Type, ResType};
                  {false, true} -> {decode, ResType, Type};
                  _ -> throw("invalid types combination")
                  end,
      
      BinTypeInteger = lists:member(BinType, ?LIST_BINARY_INT_NERLTENSOR_TYPE),
      BinTypeFloat = lists:member(BinType, ?LIST_BINARY_FLOAT_NERLTENSOR_TYPE),
      
      % Wrong combination guard
      case ErlType of 
      erl_float when BinTypeFloat-> ok;
      erl_int when BinTypeInteger -> ok;
      _ -> throw("invalid types combination")
      end,
      
      case Operation of 
            encode -> encode_nif(NerlTensor, BinType);
            decode -> decode_nif(NerlTensor, BinType);
            _ -> throw("wrong operation")
      end.

%% get BinType (float, double...) -> ErlType (erl_float / erl_int)
erl_type_conversion(BinType) ->
      {_, ErlType} = lists:keyfind(BinType, 1, ?NERL_TYPES),
      ErlType.

nerltensor_sum_erl({NerlTensorErlA, Type}, {NerlTensorErlB, Type}) ->
      ListGroup = lists:member(Type, get_all_nerltensor_list_types()),
      if ListGroup ->
            Dims = lists:sublist(NerlTensorErlA, 1, ?NUMOF_DIMS),
            NerlTensorErlA_NODIMS = lists:sublist(NerlTensorErlA, ?NUMOF_DIMS + 1, length(NerlTensorErlA) - ?NUMOF_DIMS),
            %io:format("nerltensorA nodims: ~p~n", [NerlTensorErlA_NODIMS]),
            NerlTensorErlB_NODIMS = lists:sublist(NerlTensorErlB, ?NUMOF_DIMS + 1, length(NerlTensorErlB) - ?NUMOF_DIMS),
           % io:format("nerltensorB nodims: ~p~n", [NerlTensorErlB_NODIMS]),
            Dims ++ lists:zipwith(fun(X,Y) -> X + Y end, NerlTensorErlA_NODIMS, NerlTensorErlB_NODIMS);
         true -> throw("Bad Type")
      end.

nerltensor_scalar_multiplication_erl({NerlTensorErl, Type}, ScalarValue) -> 
      ListGroup = lists:member(Type, get_all_nerltensor_list_types()),
      if 
            ListGroup ->
                  Dims = lists:sublist(NerlTensorErl, 1, ?NUMOF_DIMS),
                  NerlTensorErl_NODIMS = lists:sublist(NerlTensorErl, ?NUMOF_DIMS + 1, length(NerlTensorErl) - ?NUMOF_DIMS),
                  Dims ++ lists:map(fun(X) -> X * ScalarValue end, NerlTensorErl_NODIMS);
            true -> throw("Bad Type")
      end.

sum_nerltensors_lists_erl([], _ErlType) ->  throw("Zero length given to sum_nerltensors_even_lists");
sum_nerltensors_lists_erl(NerltensorList, _ErlType) when length(NerltensorList) == 1 ->  NerltensorList;
sum_nerltensors_lists_erl(NerltensorList, ErlType)  -> 
      OddLength = nerl:odd(length(NerltensorList)),
      {OddFirstElement, EvenNerltensorList} =  
      if OddLength -> {hd(NerltensorList), tl(NerltensorList)};
         true -> {[], NerltensorList}
      end,

      HalfSize = round(length(EvenNerltensorList)/2),
      % Split to high and low lists
      NerlTensorsHalfListA = lists:sublist(EvenNerltensorList, HalfSize),
      NerlTensorsHalfListB = lists:sublist(EvenNerltensorList, HalfSize + 1, HalfSize),

      % sum high and low lists
      SumResultOfTwoHalfs = lists:zipwith(fun(NerlTensorA,NerlTensorB) -> nerltensor_sum_erl({NerlTensorA, ErlType}, {NerlTensorB, ErlType}) end, NerlTensorsHalfListA, NerlTensorsHalfListB),
      % take care to the first element in case of odd length
      SumResultTwoHalfsWithOddFirst = 
      if OddLength -> [nerltensor_sum_erl({OddFirstElement, ErlType}, {hd(SumResultOfTwoHalfs), ErlType})];
      true -> SumResultOfTwoHalfs % nothing to do with first element in case of even list
      end,
      sum_nerltensors_lists_erl(SumResultTwoHalfsWithOddFirst, ErlType).

sum_nerltensors_lists([], _BinaryType) ->  throw("Zero length given to sum_nerltensors_even_lists");
sum_nerltensors_lists(NerltensorList, _BinaryType) when length(NerltensorList) == 1 ->  NerltensorList;
sum_nerltensors_lists(NerltensorList, BinaryType) -> 
      OddLength = nerl:odd(length(NerltensorList)),
      {OddFirstElement, EvenNerltensorList} =  
      if OddLength -> {hd(NerltensorList), tl(NerltensorList)};
      true -> {[], NerltensorList}
      end,

      HalfSize = round(length(EvenNerltensorList)/2),
      % Split to high and low lists
      NerlTensorsHalfListA = lists:sublist(EvenNerltensorList, HalfSize),
      NerlTensorsHalfListB = lists:sublist(EvenNerltensorList, HalfSize + 1, HalfSize),

      % sum high and low lists
      SumResultOfTwoHalfs = lists:zipwith(fun(NerlTensorA,NerlTensorB) -> element(1,nerltensor_sum_nif(NerlTensorA, NerlTensorB, BinaryType)) end, NerlTensorsHalfListA, NerlTensorsHalfListB),

      % take care to the first element in case of odd length
      SumResultTwoHalfsWithOddFirst = 
      if OddLength -> [element(1,nerltensor_sum_nif(OddFirstElement, hd(SumResultOfTwoHalfs), BinaryType))];
      true -> SumResultOfTwoHalfs % nothing to do with first element in case of even list
      end,
      sum_nerltensors_lists(SumResultTwoHalfsWithOddFirst, BinaryType).