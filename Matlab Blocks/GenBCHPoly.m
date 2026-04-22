function g = GenBCHPoly(num_poly)
%GENERATE_BCH_POLYNOMIAL Generate BCH generator polynomial
    % Проверка входного параметра
    if num_poly < 1 || num_poly > 12 || floor(num_poly) ~= num_poly
        error('num_poly must be an integer between 1 and 12');
    end
                g1 = flip([1 1 0 1 0 1 0 0 0 0 0 0 0 0 1]);
                g2 = flip([1 0 0 0 0 0 1 0 1 0 0 1 0 0 1]);
                g3 = flip([1 1 1 0 0 0 1 0 0 1 1 0 0 0 1]);
                g4 = flip([1 0 0 0 1 0 0 1 1 0 1 0 1 0 1]);
                g5 = flip([1 0 1 0 1 0 1 0 1 1 0 1 0 1 1]);
                g6 = flip([1 0 0 1 0 0 0 1 1 1 0 0 0 1 1]);
                g7 = flip([1 0 1 0 0 1 1 1 0 0 1 1 0 1 1]);
                g8 = flip([1 0 0 0 0 1 0 0 1 1 1 1 0 0 1]);
                g9 = flip([1 1 1 1 0 0 0 0 0 1 1 0 0 0 1]);
                g10 =flip([1 0 0 1 0 0 1 0 0 1 0 1 1 0 1]);
                g11 =flip([1 0 0 0 1 0 0 0 0 0 0 1 1 0 1]);
                g12 =flip([1 1 1 1 0 1 1 1 1 0 1 0 0 1 1]);

    % Создаем массив полиномов
    polys = {g1, g2, g3, g4, g5, g6, g7, g8, g9, g10, g11, g12};
    
    % Начинаем с единичного полинома
    g = 1;
    
    % Перемножаем первые num_poly полиномов
    for i = 1:num_poly
        g = conv(g, polys{i});
        g = mod(g, 2);
    end
end