function [  ] = export( file, p, t, l )
%EXPORT Summary of this function goes here
%   Detailed explanation goes here

    fid = fopen(file, 'w');
    
    fprintf(fid, 'v %f %f\n', p');
    fprintf(fid, 't %i %i %i\n', t');
    
    if nargin == 4
        for i = 1:length(l)
           v = [t(i,:), l(i)];
           fprintf(fid, 'l %i %i %i %i\n', v);
        end
    end
    %fprintf(fid, 'e'); % end of file (shouldn't be necessary, but it is for now)
    
    fclose(fid);
end