classdef PLS_Codec < matlab.System
    % PLS_Codec - Генерация 64-битного PLS кода (boolean выход)
    
    properties
        InputBits = [0 0 0 0 0 0 0];
    end
    
    properties(Access = private)
        G;
    end
    
    methods(Access = protected)
        
        function setupImpl(obj)
            % Генераторная матрица G (6x32)
            obj.G = [ ...
                0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1; ...
                0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1; ...
                0 0 0 0 1 1 1 1 0 0 0 0 1 1 1 1 0 0 0 0 1 1 1 1 0 0 0 0 1 1 1 1; ...
                0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1; ...
                0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1; ...
                1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1  ...
            ];
        end
        
        function y = stepImpl(obj)
            % Проверка входных данных
            if length(obj.InputBits) ~= 7
                error('InputBits должен содержать ровно 7 бит');
            end
            
            % Кодирование
            u = obj.InputBits(1:6);
            b7 = obj.InputBits(7);
            
            % Линейное кодирование: c = u * G (mod 2)
            code_32 = mod(u * obj.G, 2);
            
            % Формирование 64 бит (boolean тип)
            y = false(1, 64);  % Логический массив
            
            for i = 1:32
                y(2*i - 1) = logical(code_32(i));
                y(2*i) = logical(mod(code_32(i) + b7, 2));
            end
        end
        
    end
    
    methods(Access = public)
        function y = getPLSCode(obj)
            y = stepImpl(obj);
        end
        
        function setInputBits(obj, bits)
            obj.InputBits = bits;
        end
    end
end