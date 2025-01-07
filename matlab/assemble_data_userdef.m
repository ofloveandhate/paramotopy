%assemble_data_userdef: a utility for putting the data from a paramotopy run into a
%vector structure, for plotting.  

% silviana amethyst
% colorado state university
% mathematics
% silvianaamethyst@gmail.com

function [data,orders] = assemble_data_userdef(solutions,info,generic_num_solns,nsolns)
%i promise solutions is used, just inside an eval call -- the editor can't see it.


data = zeros(generic_num_solns*info.mc_lines,info.numvar);
orders = zeros(generic_num_solns*info.mc_lines,1);


%to make general for any number of parameters, i use generated code
indices = '';
for ii = 1:info.numparam
	indices = sprintf('%sI%i,',indices,ii);
end
indices = indices(1:end-1);%nip off the last comma

tmp = [];
solncounter = 1;
for ii = 1:info.mc_lines
	eval( ['[' indices '] = ind2sub(info.mc_lines,ii);']);
	eval(['currlimit = nsolns(' indices ');']);
	
	for jj = 1:currlimit
		eval( ['tmp = solutions(' indices ').clusterofsoln(jj).indsoln;'])
		data(solncounter,:)=tmp(:,1) + 1i*tmp(:,2);
		orders(solncounter) = ii;
		solncounter = solncounter+1;
		
	end
end

data = data(1:solncounter-1,:);

orders = orders(1:solncounter-1);
end
