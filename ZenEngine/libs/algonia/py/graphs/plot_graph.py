import os
if not __debug__:
    import algonia_helpers

import pandas as pd
import plotly.graph_objects as go
import uuid
import base64

import sys
import webbrowser

class PlotGraph:
       
    def __init__(self, data: str):
        print('initialized')
        
    def plotGraph(self, stock_data: str):
        graph_image_base64 = None

        try:
            df = pd.read_csv(stock_data, sep=";")
            fig = go.Figure(data=go.Ohlc(x=df['date'],
                open=df['open'],
                high=df['high'],
                low=df['low'],
                close=df['close']))

            graphFileName = os.path.join(os.getcwd(),'templates','StockPrices','gui', str(uuid.uuid1()) + '.jpeg') 
            fig.write_image(graphFileName)

            with open(graphFileName, "rb") as img_file:
                graph_image_base64 = base64.b64encode(img_file.read())
            
            return graph_image_base64
        except Exception as inst:
            print('error')
            print(type(inst))
            print(inst.args)
            print(inst)    
            return 'error' 