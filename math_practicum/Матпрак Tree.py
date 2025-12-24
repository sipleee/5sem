import matplotlib.pyplot as plt
import numpy as np

transformations = [
    {"a": 0.00, "b": 0.00, "c": 0.00, "d": 0.50, "e": 0.00, "f": 0.00, "p": 0.05},
    {"a": 0.42, "b": -0.42, "c": 0.42, "d": 0.42, "e": 0.00, "f": 0.20, "p": 0.40},
    {"a": 0.42, "b": 0.42, "c": -0.42, "d": 0.42, "e": 0.00, "f": 0.20, "p": 0.40},
    {"a": 0.10, "b": 0.00, "c": 0.00, "d": 0.10, "e": 0.00, "f": 0.20, "p": 0.15}
]

# Случайный выбор на основе вероятностей
probabilities = [t["p"] for t in transformations]

# Начальные параметры для построения
n_points = 200000  # количество точек
x, y = 0, 0  # начальная точка

# Списки для хранения координат точек фрактала
x_points, y_points = [], []

# Генерация точек фрактала
for _ in range(n_points):
    # Выбор преобразования на основе вероятности
    transform = np.random.choice(transformations, p=probabilities)
    
    # Вычисление новых координат
    x_new = transform["a"] * x + transform["b"] * y + transform["e"]
    y_new = transform["c"] * x + transform["d"] * y + transform["f"]
    
    # Добавление новых координат в списки
    x, y = x_new, y_new
    x_points.append(x)
    y_points.append(y)

# Визуализация фрактала
plt.figure(figsize=(10, 10))
plt.scatter(x_points, y_points, s=0.1, color="green", marker=".")
plt.axis("equal")
plt.axis("off")
plt.show()
