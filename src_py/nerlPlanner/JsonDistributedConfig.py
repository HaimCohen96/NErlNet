from JsonElements import *
from collections import OrderedDict
from JsonElementWorker import *

#   from collections import OrderedDict
#values=json.loads(jsontext,object_pairs_hook=OrderedDict)

KEY_NERLNET_SETTINGS = "NerlNetSettings"
KEY_DEVICES = "devices"
KEY_CLIENTS = "clients"
KEY_WORKERS = "workers"
KEY_WORKERS_SHA = "workers_sha"
KEY_SOURCES = "sources"
KEY_ROUTERS = "routers"

class JsonDistributedConfig():
    def __init__(self):
        self.main_dict = OrderedDict()
        self.ports_set = set()
        self.special_entities_list = [MainServer.NAME, ApiServer.NAME]
        self.reserved_names_set = set(self.special_entities_list)
        self.init_dictionary()
        self.default_frequency = None
    
    def init_dictionary(self):
        self.main_dict[KEY_NERLNET_SETTINGS] = ""
        self.main_dict[MainServer.NAME] = ""
        self.main_dict[ApiServer.NAME] = ""
        self.main_dict[KEY_DEVICES] = OrderedDict()
        self.main_dict[KEY_ROUTERS] = OrderedDict()
        self.main_dict[KEY_SOURCES] = OrderedDict()
        self.main_dict[KEY_CLIENTS] = OrderedDict()
        self.main_dict[KEY_WORKERS] = OrderedDict()
        self.main_dict[KEY_WORKERS_SHA] = {}

    def clear(self):
        self.init_dictionary()

    def get_entities(self):
        special_entities = [x for x in list(self.reserved_names_set) if x in self.main_dict]
        return self.get_clients_names() + self.get_routers_names() + self.get_sources_names() + special_entities

    def add_nerlnet_settings(self, frequency : Frequency, batchsize : BatchSize):
        self.default_frequency = frequency
        settings_dict_content = [frequency.get_as_tuple(), batchsize.get_as_tuple()]
        self.main_dict[KEY_NERLNET_SETTINGS] = OrderedDict(settings_dict_content)

    def get_frequency(self):
        return self.default_frequency # returns Frequency or None

    def add_main_server(self, main_server : MainServer):
        self.main_dict[MainServer.NAME] = main_server.get_as_dict()

    def add_api_server(self, api_server : ApiServer):
        self.main_dict[ApiServer.NAME] = api_server.get_as_dict()    

    def add_client(self, client : Client): 
        '''
        return false if name is being used
        '''     
        client_name = client.get_name()
        if client_name in self.get_entities():
            return False
        if client_name in self.reserved_names_set:
            raise "reserved name is being used with a client!"
        else:
            self.main_dict[KEY_CLIENTS][client_name] = client
        return True
    
    def get_clients_names(self):
        return list(self.main_dict[KEY_CLIENTS].keys())
    
    def get_owned_workers_by_clients_dict(self) -> list:
        owned_workers_dict = {}
        for client_name , client in self.main_dict[KEY_CLIENTS].items():
            for worker in client.get_workers_names():
                owned_workers_dict[worker] = client_name
        return owned_workers_dict

    def get_entity(self, entity_name : str, entity_type):
        if entity_type in [KEY_ROUTERS, KEY_CLIENTS, KEY_SOURCES]:
            return self.main_dict[entity_type][entity_name] if entity_name in self.main_dict[entity_type] else None
        raise f"bad entity type {entity_type} is not one of KEY_ROUTERS, KEY_CLIENTS, KEY_SOURCE"

    def get_client(self, client_name : str) -> Client: 
        return self.get_entity(client_name, KEY_CLIENTS)

    def add_router(self, router : Router): 
        '''
        return false if name is being used
        '''      
        router_name = router.get_name()
        if router_name in self.get_entities():
            return False
        if router_name in self.reserved_names_set:
            raise "reserved name is being used with a client!"
        else:
            self.main_dict[KEY_ROUTERS][router_name] = router
        return True
    
    def get_router(self, router_name : str) -> Router:
        return self.get_entity(router_name, KEY_ROUTERS)
    
    def get_routers_names(self):
        return list(self.main_dict[KEY_ROUTERS].keys())
    
    def add_source(self, source : Source): 
        '''
        return false if name is being used
        '''
        source_name = source.get_name()
        if source_name in self.get_entities():
            return False
        if source_name in self.reserved_names_set:
            raise "reserved name is being used with this client!"
        else:
            self.main_dict[KEY_SOURCES][source_name] = source
        return True
    
    def get_source(self, source_name : str) -> Source:
        return self.get_entity(source_name, KEY_SOURCES)
    
    def get_sources_names(self):
        return list(self.main_dict[KEY_SOURCES].keys())

    def add_worker(self, worker : Worker):
        worker_name = worker.get_name()
        workers_names_list = list(self.main_dict[KEY_WORKERS].keys())
        if worker_name in self.get_entities():
            return False
        if worker_name in workers_names_list:
            return False
        if worker_name in self.reserved_names_set:
            raise "reserved name is being used with this worker!"
        else: 
            self.main_dict[KEY_WORKERS][worker_name] = worker # Save the woker instance into dictionary
            worker_sha = worker.get_sha() # generate sha of worker
            if worker_sha not in self.main_dict[KEY_WORKERS_SHA]:
                self.main_dict[KEY_WORKERS_SHA][worker_sha] = [worker_name]
            else:
                self.main_dict[KEY_WORKERS_SHA][worker_sha].append(worker_name)
        return True
    
    def get_workers_dict(self):
        return self.main_dict[KEY_WORKERS]

    def get_workers_names_list(self):
        return list(self.main_dict[KEY_WORKERS].keys())

    def reserved_name(self, name):
        return name in self.reserved_names_set