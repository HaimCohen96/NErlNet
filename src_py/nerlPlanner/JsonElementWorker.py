from hashlib import sha256
import graphviz as gviz
import pydot
import json
from collections import OrderedDict

from JsonElements import JsonElement
from JsonElementsDefinitions import *
from JsonElementWorkerDefinitions import *


class Worker(JsonElement):      
    def __init__(self, name, LayersSizesListStr : str, ModelTypeStr : str, ModelType : int, OptimizationTypeStr : str, OptimizationType : int,
                 LossMethodStr : str, LossMethod : int, LearningRate : str, LayersFunctionsCodesListStr : str, LayerTypesListStr : str):
        super(Worker, self).__init__(name, WORKER_TYPE)
        self.LayersSizesListStr = LayersSizesListStr
        self.LayersSizesList = LayersSizesListStr.split(',')
        self.ModelTypeStr = ModelTypeStr
        self.ModelType = ModelType # None
        self.OptimizationTypeStr = OptimizationTypeStr
        self.OptimizationType = OptimizationType # None
        self.LossMethodStr = LossMethodStr
        self.LossMethod = LossMethod # None
        self.LearningRate = float(LearningRate)
        self.LayersFunctionsCodesListStr = LayersFunctionsCodesListStr
        self.LayersFunctionsCodesList = LayersFunctionsCodesListStr.split(',') #TODO validate
        self.LayerTypesListStr = LayerTypesListStr
        self.LayerTypesList = LayerTypesListStr.split(',') #TODO validate

        # validate lists sizes 
        lists_for_length = [self.LayersSizesList, self.LayersFunctionsCodesList, self.LayerTypesList]
        list_of_lengths = [len(x) for x in lists_for_length]
        self.lengths_validation = all([x == list_of_lengths[0] for x in list_of_lengths])

    def generate_graphviz(self):
        self.layers_graph = gviz.Digraph()   
        self.layers_graph.graph_attr['fontname'] = "helvetica"
        self.layers_graph.node_attr['fontname'] = "helvetica"
        self.layers_graph.edge_attr['fontname'] = "helvetica"

        # create nodes
        for curr_layer_idx, curr_layer_size in enumerate(self.LayersSizesList):
            curr_layer_type_num = self.LayerTypesList[curr_layer_idx]
            curr_layer_function_num = self.LayersFunctionsCodesList[curr_layer_idx]
            curr_layer_function_str = ""

            get_layer_type_str = get_key_by_value(LayerTypeMap,curr_layer_type_num)
            layer_type_dict = LayerTypeToFunctionalMap[get_layer_type_str]

            if layer_type_dict:
                curr_layer_function_str = get_key_by_value(layer_type_dict, curr_layer_function_num)

            label = f'Type: {get_layer_type_str} Size: {curr_layer_size} Func: {curr_layer_function_str}'

            self.layers_graph.node(str(curr_layer_idx),label=label, shape='Mrecord',fontsize=str(13), labelfontsize=str(13))
        
        for curr_layer_idx in range(0,len(self.LayersSizesList)-1):
            self.layers_graph.edge(str(curr_layer_idx), str(curr_layer_idx+1))
        
        return self.layers_graph

    def save_graphviz(self,path):
        filename_dot = f"{path}/worker_graph_{self.get_sha()}.dot"
        filename_png = f"{path}/worker_graph_{self.get_sha()}.png"

        layers_graph = self.generate_graphviz()
        layers_graph.save(filename_dot)

        (graph,) = pydot.graph_from_dot_file(filename_dot)
        graph.write_png(filename_png)
        return filename_png

    def copy(self, name):
        newWorker =  Worker(name, self.LayersSizesListStr, self.ModelTypeStr, self.ModelType , self.OptimizationTypeStr, self.OptimizationType,
                 self.LossMethodStr, self.LossMethod, self.LearningRate, self.LayersFunctionsCodesListStr, self.LayerTypesListStr)
        return newWorker

    def __str__(self):
        return f"layers sizes: {self.LayersSizesListStr}, model {self.ModelTypeStr}, using optimizer {self.OptimizationTypeStr}, loss method: {self.LossMethodStr}, lr: {self.LearningRate}"
    
    def error(self): 
        return not self.input_validation() # + more checks

    def input_validation(self):
        # TODO add more validation: e.g., numbers of keys appears in dictionaries
        return self.lengths_validation
    
    def get_as_dict(self, documentation = True):
        assert not self.error()
        self.key_val_pairs = [
            (KEY_MODEL_TYPE, self.ModelType),
            (KEY_MODEL_TYPE_DOC, VAL_MODEL_TYPE_DOC),
            (KEY_LAYER_SIZES_LIST, self.LayersSizesListStr),
            (KEY_LAYER_SIZES_DOC, VAL_LAYER_SIZES_DOC),
            (KEY_LAYER_TYPES_LIST, self.LayerTypesListStr),
            (KEY_LAYER_TYPES_DOC, VAL_LAYER_TYPES_DOC),
            (KEY_LAYERS_FUNCTIONS, self.LayersFunctionsCodesListStr),
            (KEY_LAYERS_FUNCTIONS_ACTIVATION_DOC, VAL_LAYERS_FUNCTIONS_ACTIVATION_DOC),
            (KEY_LAYERS_FUNCTIONS_POOLING_DOC, VAL_LAYERS_FUNCTIONS_POOLING_DOC),
            (KEY_LAYERS_FUNCTIONS_PROBABILISTIC_DOC, VAL_LAYERS_FUNCTIONS_PROBABILISTIC_DOC),
            (KEY_LAYERS_FUNCTIONS_SCALER_DOC, VAL_LAYERS_FUNCTIONS_SCALER_DOC),
            (KEY_LOSS_METHOD, self.LossMethod),
            (KEY_LOSS_METHOD_DOC, VAL_LOSS_METHOD_DOC),
            (KEY_LEARNING_RATE, self.LearningRate),
            (KEY_LEARNING_RATE_DOC, VAL_LEARNING_RATE_DOC),
            (KEY_OPTIMIZER_TYPE, self.OptimizationType),
            (KEY_OPTIMIZER_TYPE_DOC, VAL_OPTIMIZER_TYPE_DOC)
        ]
        if not documentation:
            KEY_IDX = 0
            self.key_val_pairs = [x for x in self.key_val_pairs if KEY_DOC_PREFIX not in x[KEY_IDX]] # remove documentation keys
        self.key_val_pairs = self.dict_as_list_of_pairs_fixer(self.key_val_pairs)
        return OrderedDict(self.key_val_pairs)

    def get_sha(self):
        worker_as_str = f'{self.get_as_dict()}'
        worker_sha = sha256(worker_as_str.encode('utf-8')).hexdigest()
        return worker_sha

    def save_as_json(self, out_file : str, documentation = True):
        with open(out_file,"w") as fd_out:
            json.dump(self.get_as_dict(documentation), fd_out, indent=4)

    def load_from_dict(worker_dict : dict, name = ''):
        required_keys = [KEY_LAYER_SIZES_LIST, KEY_MODEL_TYPE, KEY_OPTIMIZER_TYPE,
                         KEY_LOSS_METHOD, KEY_LEARNING_RATE, KEY_LAYERS_FUNCTIONS,
                         KEY_LAYER_TYPES_LIST]
        
        loaded_worker = None

        all_keys_exist = all([key in worker_dict for key in required_keys])

        if all_keys_exist:
            LayersSizesList = worker_dict[KEY_LAYER_SIZES_LIST]
            ModelType = int(worker_dict[KEY_MODEL_TYPE])
            ModelTypeStr = get_key_by_value(ModelTypeMapping, worker_dict[KEY_MODEL_TYPE])
            OptimizationType = int(worker_dict[KEY_OPTIMIZER_TYPE])
            OptimizationTypeStr = get_key_by_value(OptimizerTypeMapping, worker_dict[KEY_OPTIMIZER_TYPE])
            LossMethod = int(worker_dict[KEY_LOSS_METHOD])
            LossMethodStr = get_key_by_value(LossMethodMapping, worker_dict[KEY_LOSS_METHOD])
            LearningRate = float(worker_dict[KEY_LEARNING_RATE])
            ActivationLayersList = worker_dict[KEY_LAYERS_FUNCTIONS]
            LayerTypesList = worker_dict[KEY_LAYER_TYPES_LIST]
            
            loaded_worker = Worker(name, LayersSizesList, ModelTypeStr, ModelType, OptimizationTypeStr,
                OptimizationType, LossMethodStr, LossMethod, LearningRate, ActivationLayersList, LayerTypesList)
            return loaded_worker, LayersSizesList, ModelTypeStr, ModelType, OptimizationTypeStr,\
                OptimizationType, LossMethodStr, LossMethod, LearningRate, ActivationLayersList, LayerTypesList
        
        return None