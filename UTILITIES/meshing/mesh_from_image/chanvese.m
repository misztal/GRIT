function phi = chanvese(I,max_iter,dt,nu)

[m, n] = size(I);

H = @(x) 1/2 * (1 + 2/pi .* atan(x)); % Heaviside
d = @(x) 1./(pi + x.^2);              % Dirac

phi = rand(m, n)-.5; % Initial mask

% Convert initial mask to signed distance map (reinitialization)
phi = reinit(phi);

for i = 1:max_iter
    if (mod(max_iter,max_iter/4) == 0) && (i ~= max_iter)
        phi = reinit(phi);
    end
    Hphi = H(phi);
    c1 = sum(I(:) .* Hphi(:))/sum(Hphi(:));
    c2 = sum(I(:) .* (1-Hphi(:)))/sum(1-Hphi(:));
    
    force = d(phi) .* ((I-c2).^2 - (I-c1).^2 - nu*Hphi);
    oldphi = phi;
    phi = oldphi + dt.*force;
end

phi = reinit(phi);


end