VERSION = "1.0.0"
NERLNET_VERSION_TESTED_WITH = "1.2.0"
NERLNET_TMP_PATH = "/tmp/nerlnet"
NERLNET_GRAPHVIZ_OUTPUT_DIR = f"{NERLNET_TMP_PATH}/nerlplanner"
NERLNET_GLBOAL_PATH = "/usr/local/lib/nerlnet-lib/NErlNet"
NERL_PLANNER_PATH = NERLNET_GLBOAL_PATH+"/src_py/nerlPlanner"
NERLNET_LOGO_PATH = NERL_PLANNER_PATH+"/NerlnetIcon.png"
NERLNET_SPLASH_LOGO_PATH = NERL_PLANNER_PATH+"/Nerlnet_splash_logo.png"
WINDOW_TITLE = "NerlPlanner"

JSON_CONTROL_LOAD_FILE_BROWSE_EVENT_KEY = 'JSON_CONTROL_LOAD_FILE_BROWSE_EVENT_KEY'
JSON_CONTROL_EXPORT_BROWSE_EVENT_KEY = 'JSON_CONTROL_EXPORT_BROWSE_EVENT_KEY'
WIN_WORKER_DIALOG_EVENT_KEY = 'WIN_WORKER_DIALOG'


KEY_SETTINGS_FREQUENCY_INPUT = '-KEY-SETTINGS-FREQUENCY-INPUT-'
KEY_SETTINGS_BATCH_SIZE_INPUT = '-KEY-SETTINGS-BATCH-SIZE-INPUT-'
KEY_SETTINGS_MAINSERVER_IP_INPUT = '-KEY-SETTINGS-MAINSERVER-IP-INPUT-'
KEY_SETTINGS_MAINSERVER_PORT_INPUT = '-KEY-SETTINGS-MAINSERVER-PORT-INPUT-'
KEY_SETTINGS_MAINSERVER_ARGS_INPUT = '-KEY-SETTINGS-MAINSERVER-ARGS-INPUT-'
KEY_SETTINGS_APISERVER_IP_INPUT = '-KEY-SETTINGS-APISERVER-IP-INPUT-'
KEY_SETTINGS_APISERVER_PORT_INPUT = '-KEY-SETTINGS-APISERVER-PORT-INPUT-'
KEY_SETTINGS_APISERVER_ARGS_INPUT = '-KEY-SETTINGS-APISERVER-ARGS-INPUT-' 
KEY_SETTINGS_SAVE_BUTTON = '-KEY-SETTINGS-SAVE-BUTTON-'

KEY_WORKERS_INPUT_LOAD_WORKER_PATH = '-KEY-WORKERS-INPUT-LOAD-WORKER-PATH-'
KEY_WORKERS_LOAD_FROM_LIST_WORKER_BUTTON = '-KEY-WORKERS-LOAD-FROM-LIST-WORKER-BUTTON-'
KEY_WORKERS_SHOW_WORKER_BUTTON = '-KEY-WORKERS-SHOW-WORKER-BUTTON-'
KEY_WORKERS_NAME_INPUT = '-KEY-WORKERS-NAME-INPUT-'
KEY_WORKERS_BUTTON_ADD = '-KEY-WORKERS-BUTTON-ADD-'
KEY_WORKERS_BUTTON_VIEW = '-KEY-WORKERS-BUTTON-VIEW-'
KEY_WORKERS_BUTTON_REMOVE = '-KEY-WORKERS-BUTTON-REMOVE-'
KEY_WORKERS_LIST_BOX = '-KEY-WORKERS-LIST-BOX-'
KEY_WORKERS_INFO_BAR = '-KEY-WORKERS-INFO-BAR-'

KEY_DEVICES_SCANNER_BUTTON = '-KEY-DEVICES-SCANNER-BUTTON-'
KEY_DEVICES_ONLINE_LIST_COMBO_BOX = '-KEY-DEVICES-ONLINE-LIST-COMBO-BOX-'
KEY_DEVICES_SCANNER_INPUT_LAN_MASK = '-KEY-DEVICES-SCANNER-INPUT-LAN-MASK-'
KEY_DEVICES_IP_INPUT = '-KEY-DEVICES-IP-INPUT-'
KEY_DEVICES_NAME_INPUT = '-KEY-DEVICES-NAME-'

KEY_CLIENTS_WORKERS_LIST_COMBO_BOX = '-KEY-CLIENTS-WORKERS-LIST-COMBO-BOX-'
KEY_CLIENTS_BUTTON_ADD = '-KEY-CLIENTS-BUTTON-ADD-'
KEY_CLIENTS_BUTTON_LOAD = '-KEY-CLIENTS-BUTTON-LOAD-'
KEY_CLIENTS_BUTTON_REMOVE = '-KEY-CLIENTS-BUTTON-REMOVE-'
KEY_CLIENTS_NAME_INPUT = '-KEY-CLIENTS-NAME-INPUT-'
KEY_CLIENTS_PORT_INPUT = '-KEY-CLIENTS-PORT-INPUT-'
KEY_CLIENTS_WORKERS_LIST_ADD_WORKER = '-KEY-CLIENTS-WORKERS-LIST-ADD-WORKER-'
KEY_CLIENTS_WORKERS_LIST_REMOVE_WORKER = '-KEY-CLIENTS-WORKERS-LIST-REMOVE-WORKER-'
KEY_CLIENTS_WORKERS_LIST_BOX_CLIENT_FOCUS = '-KEY-CLIENTS-WORKERS-LIST-BOX-CLIENT-FOCUS-'


KEY_ROUTERS_BUTTON_ADD = '-KEY-ROUTERS-BUTTON-ADD-'
KEY_ROUTERS_BUTTON_LOAD = '-KEY-ROUTERS-BUTTON-LOAD-'
KEY_ROUTERS_BUTTON_REMOVE = '-KEY-ROUTERS-BUTTON-REMOVE-'
KEY_ROUTERS_NAME_INPUT = '-KEY-ROUTERS-NAME-INPUT-'
KEY_ROUTERS_PORT_INPUT = '-KEY-ROUTERS-PORT-INPUT-'
KEY_ROUTERS_POLICY_COMBO_BOX = '-KEY-ROUTERS-POLICY-COMBO-BOX-'

KEY_SOURCES_BUTTON_ADD = '-KEY-SOURCES-BUTTON-ADD-'
KEY_SOURCES_BUTTON_LOAD = '-KEY-SOURCES-BUTTON-LOAD-'
KEY_SOURCES_BUTTON_REMOVE = '-KEY-SOURCES-BUTTON-REMOVE-'
KEY_SOURCES_NAME_INPUT = '-KEY-SOURCES-NAME-INPUT-'
KEY_SOURCES_PORT_INPUT = '-KEY-SOURCES-PORT-INPUT-'
KEY_SOURCES_FREQUENCY_INPUT = '-KEY-SOURCES-FREQUENCY-INPUT-'
KEY_SOURCES_FREQUENCY_DEFAULT_CHECKBOX = '-KEY-SOURCES-FREQUENCY-DEFAULT-CHECKBOX-'
KEY_SOURCES_EPOCHS_INPUT = '-KEY-SOURCES-EPOCHS-INPUT-'
KEY_SOURCES_POLICY_COMBO_BOX = '-KEY-SOURCES-POLICY-COMBO-BOX-'


KEY_ENTITIES_CLIENTS_LISTBOX = '-KEY-ENTITIES-CLIENTS-LISTBOX-'
KEY_ENTITIES_ROUTERS_LISTBOX = '-KEY-ENTITIES-ROUTERS-LISTBOX-'
KEY_ENTITIES_SOURCES_LISTBOX = '-KEY-ENTITIES-SOURCES-LISTBOX-'
KEY_CLIENTS_STATUS_BAR = '-KEY-CLIENTS-STATUS-BAR-'

KEY_DEVICES_SELECTED_ENTITY_COMBO = '-KEY-DEVICES-SELECTED-ENTITY-COMBO-'


def pretty_print_dict(d):#define d
    pretty_dict = ''  #take empty string
    for k, v in d.items():#get items for dict
        pretty_dict += f'{k}: {str(v)}\n'
    return pretty_dict#return result

def print_banner():
    print("\n d8b   db d88888b d8888b. db      d8b   db d88888b d888888b\n \
888o  88 88'     88  `8D 88      888o  88 88'     `~~88~~'\n \
88V8o 88 88ooooo 88oobY' 88      88V8o 88 88ooooo    88   \n \
88 V8o88 88~~~~~ 88`8b   88      88 V8o88 88~~~~~    88   \n \
88  V888 88.     88 `88. 88booo. 88  V888 88.        88   \n \
VP   V8P Y88888P 88   YD Y88888P VP   V8P Y88888P    YP   \n \
                                                          \n \
                                                          \n \
d8888b. db       .d8b.  d8b   db d8b   db d88888b d8888b. \n \
88  `8D 88      d8' `8b 888o  88 888o  88 88'     88  `8D \n \
88oodD' 88      88ooo88 88V8o 88 88V8o 88 88ooooo 88oobY' \n \
88~~~   88      88~~~88 88 V8o88 88 V8o88 88~~~~~ 88`8b   \n \
88      88booo. 88   88 88  V888 88  V888 88.     88 `88. \n \
88      Y88888P YP   YP VP   V8P VP   V8P Y88888P 88   YD \n \
                                                          \n \
                                                          \n ")
    print(f"Nerlnet Planner version {VERSION} is given without any warranty.")
    print(f"There is no commitiment or responsibility for results, damage, loss that can be caused by using this tool.")
    print(f"Please review the license of Nerlnet on Github repository:")
    print(f"www.github.com/leondavi/NErlNet")
    print(f"You must cite Nerlnet if you use any of its tools for academic/commercial/any purpose.")
    print(f"Tested with Nerlnet version {NERLNET_VERSION_TESTED_WITH}")