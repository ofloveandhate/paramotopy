% [data,orders] = assemble_data_real(solutions,info,nsolns)
%
%assemble_data_pos_solns: a utility for putting the parameter_points from a paramotopy run into a
%vector structure, for plotting.  
%
% returns: parameter_points, a matrix of parameter samples, 
%          color_data, a vector indicating how many positive solutions ther
%            were at the corresponding sample.
% inputs: optionally, none, loads file.
%         or, load a parameter_points set yourself, and feed this function the
%            ? solutions,
%            ? info,
%            ? nsolns.
%
% silviana amethyst
% colorado state university
% mathematics
% silvianaamethyst@gmail.com

function [data,orders] = assemble_data_real(solutions,info,generic_num_solns,nsolns)
%i promise solutions is used, just inside an eval call


data = zeros(generic_num_solns*prod(info.paramvalues(:,5)),info.numvar);
orders = zeros(generic_num_solns*prod(info.paramvalues(:,5)),1);

posrealnsolns = zeros(size(nsolns));

%to make general for any number of parameters, i use generated code
indices = '';
for ii = 1:info.numparam
	indices = sprintf('%sI%i,',indices,ii);
end
indices = indices(1:end-1);%nip off the last comma

tmp = [];
solncounter = 1;
for ii = 1:prod(info.paramvalues(:,5))
	eval( ['[' indices '] = ind2sub( (info.paramvalues(:,5))'',ii);']);
	eval(['currlimit = nsolns(' indices ');']);
	
	poscounter = 0;
	for jj = 1:currlimit
		eval( ['tmp = solutions(' indices ').clusterofsoln(jj).indsoln;'])
		
		if all(tmp(:,1)>0)
			data(solncounter,:)=tmp(:,1) + 1i*tmp(:,2);
			orders(solncounter) = ii;
			solncounter = solncounter+1;
			poscounter = poscounter+1;
		end
		eval(['posrealnsolns(' indices ') = poscounter;']);
	end
end

data = data(1:solncounter-1,:);

orders = orders(1:solncounter-1);

nsolns = posrealnsolns;
save('posrealnsolns.mat','nsolns');

end



