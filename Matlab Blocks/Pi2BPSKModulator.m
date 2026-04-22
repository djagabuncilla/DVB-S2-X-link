classdef Pi2BPSKModulator < matlab.System
    % Pi/2 BPSK Modulator
    % Модулирует входные биты в комплексные символы согласно формуле:
    % d[i] = (exp(j*pi/2 * mod(i,2)) / sqrt(2)) * ((1-2b[i]) + j(1-2b[i]))
    %
    % Вход:  вектор битов (0 или 1) — действительные числа или логические значения
    % Выход: вектор комплексных символов той же длины

    properties (Nontunable)
        InitialPhase = 0;   % Начальная чётность индекса первого символа (0 или 1)
    end

    properties (Access = private)
        Counter;            % Глобальный счётчик символов
    end

    methods (Access = protected)
        function setupImpl(obj)
            % Инициализация счётчика
            obj.Counter = obj.InitialPhase;
        end

        function y = stepImpl(obj, u)
            % u — входной вектор битов (столбец)
            u = double(u);                  % приведение к double
            if any(u ~= 0 & u ~= 1)
                error('Входные биты должны быть 0 или 1.');
            end

            N = length(u);

            % BPSK символы: +1 для 0, -1 для 1
            bpsk = 1 - 2*u;                 % вектор-столбец

            % Базовая компонента (1+j)/sqrt(2) — соответствует фазе π/4
            base = (1 + 1j) / sqrt(2);

            % Умножение на биты
            temp = bpsk .* base;            % комплексный вектор

            % Формирование поворотов для каждого символа текущего кадра
            idx = obj.Counter : (obj.Counter + N - 1);
            rotation = exp(1j * pi/2 * mod(idx, 2));   % 1 для чётных, j для нечётных
            rotation = rotation(:);                     % обеспечить форму столбца

            % Применение поворота
            y = rotation .* temp;

            % Обновление счётчика
            obj.Counter = obj.Counter + N;
        end

        function resetImpl(obj)
            % Сброс счётчика при остановке моделирования
            obj.Counter = obj.InitialPhase;
        end

        % Методы, определяющие интерфейс блока
        function num = getNumInputsImpl(~)
            num = 1;
        end

        function num = getNumOutputsImpl(~)
            num = 1;
        end

        function flag = isInputComplexImpl(~, ~)
            flag = false;   % вход действительный
        end

        function flag = isOutputComplexImpl(~, ~)
            flag = true;    % выход комплексный
        end





        function dataType = getOutputDataTypeImpl(~)
            dataType = 'double';
        end

        function size = getOutputSizeImpl(obj)
            % Выход имеет тот же размер, что и вход
            size = propagatedInputSize(obj, 1);
        end

        function validateInputsImpl(~, u)
            if ~isnumeric(u) || ~isreal(u)
                error('Вход должен быть действительным числовым массивом.');
            end
        end
    end
end