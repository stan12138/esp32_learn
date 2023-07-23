
import numpy as np
import matplotlib.pylab as mp
import matplotlib.animation as ma
import serial

ser = serial.Serial('/dev/cu.usbserial-14210', 115200)

mp.figure("Signal", facecolor='lightgray')
mp.title('Signal', fontsize=20)
mp.xlabel('Time', fontsize=14)  # 横纵坐标标签
mp.ylabel('Signal', fontsize=14)

ax = mp.gca()  # 获取坐标轴
ax.set_ylim(-3, 3)  # 垂直坐标范围
ax.set_xlim(0, 10)
mp.tick_params(labelsize=10)  # 精度值
mp.grid(linestyle=':')  # 网格线


# 创建一个plot空对象（只是没有数据，仍然是一个完整的图像）
current = mp.plot([], [], c="orangered")[0]  # 有很多个元素，此处取一个处理
current.set_data([], [])  # 设置数据，此处给的空数据，以便于之后将生成器的数据传入
aim = mp.plot([], [], c="blue")[0]
aim.set_data([], [])

# 接收生成器数据的更新函数
def update(data):
    t, v, aim_y = data  # 时间，生成器的值
    x, y = current.get_data()  # 获取生成器数据
    if v is None:
        return
    # 追加数据
    x.append(t)
    y.append(v)
    x1, y1 = aim.get_data()
    x1.append(t)
    y1.append(aim_y)

    # 移动坐标轴位置，以便持续观察数据
    # 获取当前坐标轴的最小值与最大值，即坐标系的左右边界
    x_min, x_max = ax.get_xlim()
    y_min, y_max = ax.get_ylim()

    data_y_min = min(y) - 1
    data_y_max = max(y) + 1
    change = False
    if t >= x_max:
        # 平移坐标轴：将最小值变为当前位置减去窗口宽度，最大值变为当前值
        ax.set_xlim(t - (x_max - x_min), t)
        ax.figure.canvas.draw()
    if data_y_max >= y_max or data_y_min < y_min:
        ax.set_ylim(data_y_min, data_y_max)
        ax.figure.canvas.draw()
        
    # 修改数据
    current.set_data(x, y)
    aim.set_data(x1, y1)


# 生成器函数
def generator():
    t = 0  # 时间
    while True:
        if ser.in_waiting:
            data = ser.readline().decode('utf-8').rstrip()
            try: 
                data = float(data)
            except:
                yield t, None, 2
            t += 0.05
            yield t, data, 2
        else:
            yield t, None, 2

# 生成动画
anim = ma.FuncAnimation(mp.gcf(), update, generator, interval=5)

mp.show()

