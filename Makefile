# Компилятор и флаги
CXX = cl
CXXFLAGS = /EHsc /W4 /O2
TARGET = main.exe
SOURCES = main.cpp matrix_io.cpp solver.cpp
OBJECTS = main.obj matrix_io.obj solver.obj

# Основная цель - сборка программы
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) /Fe$(TARGET)

# Компиляция отдельных объектных файлов
main.obj: main.cpp matrix_io.h solver.h
	$(CXX) $(CXXFLAGS) /c main.cpp

matrix_io.obj: matrix_io.cpp matrix_io.h
	$(CXX) $(CXXFLAGS) /c matrix_io.cpp

solver.obj: solver.cpp solver.h
	$(CXX) $(CXXFLAGS) /c solver.cpp

# Очистка скомпилированных файлов
clean:
	del /Q $(OBJECTS) $(TARGET) *.pdb *.ilk

# Запуск программы с тестовыми параметрами
run: $(TARGET)
	.\$(TARGET) 3 3 1

# Показываем справку
help:
	@echo Доступные команды:
	@echo   nmake        - Собрать программу
	@echo   nmake run    - Собрать и запустить с тестовыми параметрами
	@echo   nmake clean  - Очистить скомпилированные файлы

.PHONY: clean run help