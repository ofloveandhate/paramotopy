% plot_numsolns_pos_real(parameter_points, num_real_pos_solns, info)
%
% a utility for plotting the number of real solutions corresponding to a
% parameter sample, for paramotopy.


% silviana amethyst
% colorado state university
% mathematics
% 2013
% silvianaamethyst@gmail.com

function plot_numsolns_pos_real(parameter_points, num_real_pos_solns, info)

plot_params.fontsize = 16;
plot_params.window = [40 40 800 600];
plot_params.format = 'eps';
plot_params.format_flag = 'epsc2';
fontsize = plot_params.fontsize;

if nargin<3
	if ~isempty(dir('posreal.mat'));
		display('loading data');
		load('posreal.mat');
	else
		display('no data!\nplease run ''assemble_data_pos_solns'' on a real_solutions data set.');
		return;
	end
end

dataname = 'posreal';

if info.userdefined==1
	num_points = info.mc_lines;
else
	num_points = prod(info.paramvalues(:,5));
end


skip = [];
if num_points>10000
	while isempty(skip)
		display(sprintf('the sample to plot has %i points.\n',num_points));
		skip = input('by what integer factor would you like to reduce the size of the sample?\n1 is no reduction (the full data set)\n: ');
	end
else
	skip = 1;
end

maxiii = max_recursive(num_real_pos_solns);

parameter_points = parameter_points(1:skip:end,:);
num_real_pos_solns = num_real_pos_solns(1:skip:end);
		
if maxiii~=0
	num_real_pos_solns = num_real_pos_solns/maxiii;
end

switch info.numparam
	case {1}

		
	case {2}
		h = scatter(parameter_points(:,1),parameter_points(:,2),20,num_real_pos_solns);

		xlabel(info.paramnames{1},'FontSize',fontsize);
		ylabel(info.paramnames{2},'FontSize',fontsize);
		
	case {3}
		h = scatter3(parameter_points(:,1),parameter_points(:,2),parameter_points(:,3),20*num_real_pos_solns,num_real_pos_solns);

		xlabel(info.paramnames{1},'FontSize',fontsize);
		ylabel(info.paramnames{2},'FontSize',fontsize);
		zlabel(info.paramnames{3},'FontSize',fontsize);
	

	otherwise %info.numparam>3
		[nplot] = getuserindicestoplot_param(info);

		nplot.indices


		switch nplot.userparams


			case {1}
				%this is untested
				bar(parameter_points(:,nplot.indices(1)),num_real_pos_solns);
				xlabel(info.paramnames{nplot.indices(1)},'FontSize',fontsize);

			case {2}


				h = scatter(parameter_points(:,nplot.indices(1)),parameter_points(:,nplot.indices(2)),20,num_real_pos_solns);

				xlabel(info.paramnames{nplot.indices(1)},'FontSize',fontsize);
				ylabel(info.paramnames{nplot.indices(2)},'FontSize',fontsize);
			case {3}



				h = scatter3(parameter_points(:,nplot.indices(1)),parameter_points(:,nplot.indices(2)),parameter_points(:,nplot.indices(3)),20,num_real_pos_solns);

				xlabel(info.paramnames{nplot.indices(1)},'FontSize',fontsize);
				ylabel(info.paramnames{nplot.indices(2)},'FontSize',fontsize);
				zlabel(info.paramnames{nplot.indices(3)},'FontSize',fontsize);
			otherwise  %more than 3 variables

		end

end

make_nsolns_colorbar(maxiii,plot_params.fontsize);

axis tight
render_into_file(sprintf('numposreal_%s',info.filename),plot_params);
end