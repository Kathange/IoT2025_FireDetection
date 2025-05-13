from flask import Flask, render_template, url_for, request
import matplotlib
matplotlib.use('Agg')  # 使用非交互式后端 Agg
import pandas as pd
import matplotlib.pyplot as plt
import os
import json
from firebase_admin import credentials, db, initialize_app

# 初始化 Firebase
cred = credentials.Certificate("iot2025fire-firebase-adminsdk-fbsvc-7534bd2067.json")
initialize_app(cred, {
    'databaseURL': 'https://iot2025fire-default-rtdb.firebaseio.com'
})

app = Flask(__name__)

import os
import json
import pandas as pd
from firebase_admin import db

def download_and_save_data():
    # 从 Firebase 获取数据
    ref = db.reference('/sensors')
    data = ref.get()
    
    # 打印数据确认结构
    print(json.dumps(data, indent=4))  # 美化输出，便于查看结构
    
    # 保存数据到本地 JSON 文件
    with open('db.json', 'w') as file:
        json.dump(data, file, ensure_ascii=False, indent=4)


def get_data():
    # 如果本地文件不存在，则从 Firebase 下载数据
    if not os.path.exists('db.json'):
        print("本地 JSON 文件不存在，正在从 Firebase 下载数据...")
        download_and_save_data()
    else:
        print("使用本地 JSON 文件。")

    # 读取本地 JSON 文件
    with open('db.json', 'r') as file:
        data = json.load(file)

    records = []
    for date in data:
        for time in data[date]:
            row = data[date][time]
            timestamp = pd.to_datetime(date + time, format="%Y%m%d%H%M%S")
            row['timestamp'] = timestamp
            records.append(row)

    df = pd.DataFrame(records)
    df.set_index('timestamp', inplace=True)
    return df


def create_plot(df, metric):
    plt.figure(figsize=(10, 5))
    
    # 根据不同的metric选择不同的列来画图
    if metric == 'temperature':
        plt.plot(df.index, df['temperature'], label='Temperature (°C)', color='red', marker='o')
        plt.title('Temperature')
        plt.ylabel('Temperature (°C)')
    elif metric == 'aoEstimate':
        plt.plot(df.index, df['aoEstimate'], label='AO Estimate (V)', color='orange', marker='x')
        plt.title('AO Estimate')
        plt.ylabel('AO Estimate (V)')
    elif metric == 'mq2_analog':
        plt.plot(df.index, df['mq2_analog'], label='MQ2 Analog', color='green', marker='s')
        plt.title('MQ2 AO')
        plt.ylabel('MQ2 AO')
    elif metric == 'flame':
        # 填充 NaN 值为 0，然后将布尔值转换为 1 和 0
        df['flame'] = df['flame'].fillna(False).astype(int)  # 填充 NaN 为 False，转换为整数
        plt.plot(df.index, df['flame'], label='Flame (True/False)', color='blue', marker='^', linestyle='-', markersize=5)
        plt.title('Fire Flame')
        plt.ylabel('Fire (1=True, 0=False)')
    elif metric == 'gas_alert':
        # 填充 NaN 值为 0，然后将布尔值转换为 1 和 0
        df['gas_alert'] = df['gas_alert'].fillna(False).astype(int)  # 填充 NaN 为 False，转换为整数
        plt.plot(df.index, df['gas_alert'], label='Gas Alert (True/False)', color='purple', marker='D', linestyle='-', markersize=5)
        plt.title('MQ2 Gas Alert')
        plt.ylabel('MQ2 DO (1=True, 0=False)')

    plt.xlabel('time')
    plt.legend()
    plt.grid(True)
    plt.tight_layout()

    # 確保 static 資料夾存在
    if not os.path.exists('static'):
        os.makedirs('static')

    plt.savefig(f'static/{metric}_plot.png')
    plt.close()

def get_alert_data(df):
    # 过滤出火焰和气体警报都为 True 的记录
    alert_df = df[(df['flame'] == True) & (df['gas_alert'] == True)]
    return alert_df

@app.route('/', methods=['GET', 'POST'])
def index():
    df = get_data()

    # 获取警报数据
    alert_df = get_alert_data(df)
    
    # 默认选择显示温度图
    metric = request.args.get('metric', 'temperature')

    create_plot(df, metric)

    # 将警报数据传递到模板
    return render_template('index.html',
                           plot_url=url_for('static', filename=f'{metric}_plot.png'),
                           alert_data=alert_df.to_html(classes="table table-striped", index=True))  # 传递警报数据


if __name__ == '__main__':
    app.run(debug=True)
