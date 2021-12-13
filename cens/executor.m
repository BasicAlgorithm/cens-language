function cens_neuron_model
Vrest = 0; % mV − change this to −65 if desired
dt = 0.01; % ms
totalTime = 150; % ms
C = 1; % uF/cm^2
% constants; values based on Table 1
E_Na = 115 + Vrest; % mV
E_K = -6 + Vrest; %mV
E_Leak = 10.6 + Vrest; % mV
g_Na = 120; % mS/cm^2
g_K = 36; % mS/cm^2
g_Leak = 0.3; % mS/cm^2
% Vector of timesteps
t = [0:dt:totalTime];
% Current input −− change this to see how different inputs affect the neuron
I_current = ones(1,length(t))*0.0;
I_current(50/dt:end) = 3; 
% Input of 3 microA/cm2 beginning at 50 ms and steady until end of time period.
% initializing values
V(1) = Vrest; % membrane potential is starting at its resting state
% separate functions to get the alpha and beta values
[alphaM, betaM] = m_equations(V(1), Vrest);
[alphaN, betaN] = n_equations(V(1), Vrest);
[alphaH, betaH] = h_equations(V(1), Vrest);
% initializing gating variables to the asymptotic values when membrane potential
% is set to the membrane resting value based on equation 13
m(1) = (alphaM / (alphaM + betaM));
n(1) = (alphaN / (alphaN + betaN));
h(1) = (alphaH / (alphaH + betaH));
% repeat for time determined in totalTime , by each dt
for i = 1:length(t)
    % calculate new alpha and beta based on last known membrane potenatial
    [alphaN, betaN] = n_equations(V(i), Vrest);
    [alphaM, betaM] = m_equations(V(i), Vrest);
    [alphaH, betaH] = h_equations(V(i), Vrest);
    % conductance variables − computed separately to show how this
    % changes with membrane potential in one of the graphs
    conductance_K(i) = g_K*(n(i)^4);
    conductance_Na(i)=g_Na*(m(i)^3)*h(i);
    % retrieving ionic currents
    I_Na(i) = conductance_Na(i)*(V(i)-E_Na);
    I_K(i) = conductance_K(i)*(V(i)-E_K);
    I_Leak(i) = g_Leak*(V(i)-E_Leak);
    % Calculating the input
    Input = I_current(i) - (I_Na(i) + I_K(i) + I_Leak(i));
    % Calculating the new membrane potential
    V(i+1) = V(i) + Input* dt*(1/C);
    % getting new values for the gating variables
    m(i+1) = m(i) + (alphaM *(1-m(i)) - betaM * m(i))*dt;
    n(i+1) = n(i) + (alphaN *(1-n(i)) - betaN * n(i))*dt;
    h(i+1) = h(i) + (alphaH *(1-h(i)) - betaH * h(i))*dt;
    end

figure('Name', 'Gating Parameters')
plot(t(45/dt:end),m(45/dt:end-1), 'r',t(45/dt:end), n(45/dt:end-1), 'b',t(45/dt:end), h(45/dt:end-1), 'g', 'LineWidth', 2)
legend('m', 'n', 'h')
xlabel('Time (ms)')
ylabel('')
title('Gating Parameters')

figure('Name', 'Membrane Potential vs input')
subplot(2,1,1)
plot(t(45/dt:end),V(45/dt:end-1), 'LineWidth', 2)
xlabel('Time (ms)')
ylabel('Voltage (mV)')
title('Action Potential')
subplot(2,1,2)
plot(t(45/dt:end),I_current(45/dt:end), 'r', 'LineWidth', 2)
xlabel('Time (ms)')
ylabel('Voltage (mV)')
title('Input')

figure('Name', 'Conductance')
plot(t(45/dt:end),V(45/dt:end-1), 'r',t(45/dt:end), conductance_Na(45/dt:end), 'b',t(45/dt:end), conductance_K(45/dt:end), 'g', 'LineWidth', 2)
legend('Action Potential', '\ch{Na+} Conductance', '\ch{K+} Conductance')
xlabel('Time (ms)')
ylabel('Voltage (mV)')
title('Conduction of \ch{K+} and \ch{Na+}')

% Special graph to show ionic current movement
Vrest = 0;
voltage = [-100:0.01:100];
for i = 1:length(voltage)
    [alphaN, betaN] = n_equations(voltage(i), Vrest);
    [alphaM, betaM] = m_equations(voltage(i), Vrest);
    [alphaH, betaH] = h_equations(voltage(i), Vrest);
    taum(i) = 1/(alphaM+betaM);
    taun(i) = 1/(alphaN+betaN);
    tauh(i) = 1/(alphaH+betaH);
    xm(i) = alphaM/(alphaM+betaM);
    xn(i) = alphaN/(alphaN+betaN);
    xh(i) = alphaH/(alphaH+betaH);
    aN(i) = alphaN;
    bN(i) = betaN;
    aM(i) = alphaM;
    bM(i) = betaM;
    aH(i) = alphaH;
    bH(i) = betaH;
    end
figure('Name', 'Equilibrium Function');
plot(voltage, xm, voltage, xn, voltage, xh, 'LineWidth', 2);
legend('m', 'n', 'h');
title('Equilibrium Function');
xlabel('mV');
ylabel('x(u)');

end


% calculate alpha m and beta m based on Table 2
function [alpha_m, beta_m] = m_equations(V, Vrest)
alpha_m = (2.5-0.1*(V-Vrest))/(exp(2.5-0.1*(V-Vrest))-1);
beta_m = 4*exp((Vrest-V)/18);
end
% calculate alpha n and beta n based on Table 2
function [alpha_n, beta_n] = n_equations(V, Vrest)
alpha_n = (0.1-0.01*(V-Vrest))/(exp(1-0.1*(V-Vrest))-1);
beta_n = 0.125*exp((Vrest-V)/80);
end
% calculate alpha h and beta h based on Table 2
function [alpha_h, beta_h] = h_equations(V, Vrest)
alpha_h = 0.07*exp((Vrest-V)/20);
beta_h = 1/(1+exp(3-0.1*(V-Vrest)));
end
