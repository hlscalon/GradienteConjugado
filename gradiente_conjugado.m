function x = gradiente_conjugado(A,b)
	imax = 1000;
	erro = 0.00001;
	n = length(A);
	x = zeros(n,1);
	i = 1;
	r = b - A * x;
	d = r;
	sigma_novo = r' * r;
	sigma0 = sigma_novo;

	while (i < imax && sigma_novo > erro^2 * sigma0)
		q = A * d;
		alpha = sigma_novo/(d' * q);
		x = x + alpha * d;
		if ( rem(i,50) == 0 )
			r = b - A * x;
		else
			r = r - alpha * q;
		end;
		sigma_velho = sigma_novo;
		sigma_novo = r' * r;
		beta = sigma_novo / sigma_velho;
		d = r + beta * d;
		i = i + 1;
	end
end

