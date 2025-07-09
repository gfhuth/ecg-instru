import os
Import("env")

def before_upload(source, target, env):
    print("Uploading SPIFFS...")
    os.system("pio run --target uploadfs")

env.AddPreAction("upload", before_upload)
