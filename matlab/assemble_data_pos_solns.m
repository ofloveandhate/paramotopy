%[parameter_points,num_real_pos_solns] = assemble_data_pos_solns(solutions,info,nsolns)
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

function [parameter_points,num_real_pos_solns] = assemble_data_pos_solns(solutions,info,nsolns)
%i promise solutions is used, just inside an eval call

if nargin==0
	load('real_solutionsdatamatlab.mat')
end

if info.userdefined==1
	parameter_points = zeros(info.mc_lines,info.numparam);
	num_real_pos_solns = zeros(info.mc_lines,1);
	upper_limit = info.mc_lines;
	indices = 'ii';
else
	parameter_points = zeros(prod(info.paramvalues(:,5)),info.numparam);
	num_real_pos_solns = zeros(prod(info.paramvalues(:,5)),1);
	upper_limit = prod(info.paramvalues(:,5));
	
	
	%to make general for any number of parameters, i use generated code
	indices = '';
	for ii = 1:info.numparam
		indices = sprintf('%sI%i,',indices,ii);
	end
	indices = indices(1:end-1);%nip off the last comma

end




tmp = []; % initialize to prevent yelling




for ii = 1:upper_limit
	
	if info.userdefined==0
		eval( ['[' indices '] = ind2sub( (info.paramvalues(:,5))'',ii);']);
	end

	eval(['currlimit = nsolns(' indices ');']);

	eval(['parameter_points(ii,:) = locations(' indices ').location(1:2:end);']);
	num_positive = 0; % reset the counter
	for jj = 1:currlimit
		eval( ['tmp = solutions(' indices ').clusterofsoln(jj).indsoln(:,1);'])

		if all(tmp>0)
			num_positive = num_positive+1;
		end
	end

	num_real_pos_solns(ii) = num_positive;

end


save('posreal.mat','info','num_real_pos_solns','parameter_points','-v6');
display('saved to posreal.mat');


end %re : function



