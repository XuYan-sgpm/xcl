import os

def change_file(path:str):
    with open(path,'r') as file:
        for lineIdx, line in enumerate(file):
            if not line.startswith("#include "):
                continue
            

def change(path: str) -> int:
    if os.path.isfile(path):
        pass
