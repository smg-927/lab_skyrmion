#from DynamicalPhaseClass import *
from DynamicalPhaseClass_multiplier_cuda_cusparse import *

import os
import atexit
import signal
import datetime

start_time = datetime.datetime.now()

T0 = 300                                                      #temperature
 
a_STO = 3.905                                                                   #lattice parameter
a_SRO = a_STO
a_PTO = 3.955
b_PTO = 3.955

a_DSO = 3.952
b_DSO = 3.943
'''a_DSO = 3.911
b_DSO = 3.911'''


rho_SRO = 6526                                                                   #dynamiics parameter
rho_STO = 5117
rho_PTO = 7970
rho_DSO = 6780

cp_SRO = 383.08
cp_STO = 488.568
cp_PTO = 294.75
cp_DSO = 358.23 


kappa_SRO = 5.97
kappa_STO = 11.16
kappa_PTO = 5.23
kappa_DSO = 2

expan_coef_SRO = 4.5e-5
expan_coef_STO = 9e-6
expan_coef_PTO = -2.1e-5
expan_coef_DSO = 8.4e-6

atte_SRO = 30e-9
R_SRO = 0.25 # DOI: 10.1038/srep09118
LPower = 0.1*10 #J/m^2

                    #material parameters No description for alpha, G, C, Q usually used in phase field simulation
                    #kb = dielectric constant, lamb is the parameter which makes helicity
    
PTO = Ferroelectric(  {'1': 3.8 * ( T0 - 752 ) * 1e5 , '11':  -7.252e7, '12':  7.5e8,
                        '111':   2.606e8, '112':    6.1e8,  '123':    -3.7e9 ,'0':(abs(3.8 * ( 298.15 - 752 ) * 1e5)) }, # alpha, 
                    {'11' : 0.6 * 1.73e-10, '12' : 0, '44': 0.3 * 1.73e-10 ,'110' : 1.73e-10 }, # G
                    {'11' : 1.746e11 , '12':  7.937e10, '44': 1.111e11},  # C
                    {'11' : 0.089, '12': -0.026, '44': 0.0675}, # Q
                        a = a_PTO, b = b_PTO , kb = 66* 8.85e-12 , lamb = 0,
                        rho = rho_PTO , cp = cp_PTO, kappa = kappa_PTO, expan_coef = expan_coef_PTO)

STO = Ferroelectric( {'1': 4.05e7 *( np.cosh( 54 / T0 ) / np.sinh( 54 / T0 ) - 1.056 )  , '11':  17e8, '12':  4.45e9,
                        '111':  0, '112':   0,  '123':    0 ,'0':4.05e7 *( np.cosh( 54 / T0 ) / np.sinh( 54 / T0 ) - 1.056 )  }, # alpha, 
                        {'11' : 0.6 * 1.73e-10, '12' : 0, '44': 0.3 * 1.73e-10 ,'110' : 1.73e-10 }, # G
                    {'11' :330e9 , '12' : 100e9, '44':125e9},  # C
                    {'11':0.0457, '12':-0.0135, '44':0.00957}, # Q
                    a = a_STO, b= a_STO , kb = 66* 8.85e-12 , lamb = 0,
                    rho = rho_STO , cp = cp_STO, kappa = kappa_STO, expan_coef = expan_coef_STO)
            

SRO = Ferroelectric( {'1': 0  , '11':  0, '12':  0,
                        '111':  0, '112':   0,  '123':    0 ,'0':0   }, # alpha, 
                    {'11' : 0, '12' : 0, '44': 0,'110' : 0 }, # G
                    {'11' :330e9 , '12' : 100e9, '44':125e9},  # C
                    {'11':0, '12':0, '44':0} ,# Q
                        a = a_SRO , b = a_SRO , kb = 66* 8.85e-12 , lamb = 0,
                    rho = rho_SRO , cp = cp_SRO, kappa = kappa_SRO, expan_coef = expan_coef_SRO)

DSO = Ferroelectric( {'1': 0  , '11':  0, '12':  0,
                        '111':  0, '112':   0,  '123':    0 ,'0':0   }, # alpha, 
                    {'11' : 0, '12' : 0, '44': 0,'110' : 0 }, # G
                    {'11' :328e9 , '12' : 117e9, '44': 78e9},  # C
                    {'11':0, '12':0, '44':0} ,  # Q
                    a = a_DSO, b = b_DSO , kb = 66* 8.85e-12, lamb = 0 ,
                    rho = rho_DSO , cp = cp_DSO, kappa = kappa_DSO , expan_coef = expan_coef_DSO) 



#grid spacing in real spacei chosen to be deltax/le0 
#le0, which defined by square root of G110/alpha0, by Y.L. Li et al. / Acta Materialia 50 (2002) 395–411 401p
le0 = ( 1.73e-10 / abs(3.8 * ( 298.15 - 752 ) * 1e5 )) ** 0.5 
dx1 = 0.4 # dx1 value decides the real space length of one Nx1, if dx1 is 0.4, one Nx1 means 0.4nm
dx3=  0.4 # you could change dx2 value in DynamicalPhaseClass.py It is set to be same as dx1

"""L = 1 / 5e-2
t0 =0.1e-12 # 1 / PTO.alpha['0']/ L #
P0 = 0.757
dt = 1 * t0
alpha0 = PTO.alpha['0']"""


L = 1 / 5e-2
t0 = 1 / PTO.alpha['0']/ L #no dimension value of time
P0 = 0.757
dt = 0.02 * t0 # the time interval in finite difference
alpha0 = PTO.alpha['0']

mul = 0 # 1이면 multiplier로 계산, 아니고 싶다면 0
mean = 0 # 0만 아니면 mean으로 계산
#둘 다 0이면 원래 방식으로 계산

#T_P = np.array([ 3.727601504999999982e-01, 3.878838339999999829e-01, 3.961514520000000150e-01 ])

#PM = T_P[j]
PM = 0.3834365774165168

#T_P = np.arange(105, 187,1)
Nx1 = 180 # the # of units which computer will calculate, as written in above, the real length is Nx1 * dx1
Nx2 = Nx1

#sample class, which could easily put information of sample 
sample = Sample(  Nx1 = Nx1 , Nx2 = Nx2 , dx1 = dx1, dx3 = dx3 , le0 = le0 , P0 = P0, PM = PM, T0 = T0)
    
N_L = 2
PTOn = 9
# STOn = 9

#Substate
sample.AddSub( name = 'DSO',  mat = DSO , N_layer = 10 ) #10
sample.AddInt( name = 'DSO_STO_int' , mat_bot= DSO, mat_top = STO, N_layer=1)
#sample.AddSub( name = 'STO',  mat = STO , N_layer = 4 ) #add substrate, significant because strain is decided by the gap between substrate and materials, so It is necessary to add substrate


# for i in range(0,N_L): #use if you want to make superlattice
#     sample.AddLayer(name = 'STO',  mat = STO , N_layer = STOn )
#     sample.AddInt( name = 'STO_PTO_int' , mat_bot= PTO, mat_top = STO, N_layer=1)
#     sample.AddPTO(name = 'PTO',  mat = PTO , N_layer = PTOn )
#     sample.AddInt( name = 'PTO_STO_int' , mat_bot = STO, mat_top=PTO, N_layer=1)

sample.AddLayer(name = 'STO',  mat = STO , N_layer = 9 )
sample.AddInt( name = 'STO_PTO_int' , mat_bot= PTO, mat_top = STO, N_layer=1)
sample.AddPTO(name = 'PTO',  mat = PTO , N_layer = PTOn )
sample.AddInt( name = 'PTO_STO_int' , mat_bot = STO, mat_top=PTO, N_layer=1)
sample.AddLayer(name = 'STO',  mat = STO , N_layer = 30 )
sample.AddInt( name = 'STO_PTO_int' , mat_bot= PTO, mat_top = STO, N_layer=1)
sample.AddPTO(name = 'PTO',  mat = PTO , N_layer = PTOn )
sample.AddInt( name = 'PTO_STO_int' , mat_bot = STO, mat_top=PTO, N_layer=1)



#sample.AddLayer(name = 'PTO',  mat = PTO , N_layer = 4 )
sample.AddLayer(name = 'STO',  mat = STO , N_layer = 9 )
sample.AddInt( name = 'SRO_STO_int' , mat_bot= STO, mat_top = SRO, N_layer=1)
sample.AddCap(name = 'SrRuO3',  mat=SRO , N_layer =2, R= R_SRO,R_ther = 4.85e-8, atte = atte_SRO) #you shouldn't have to make cap

#os.mkdir('./SampleInfo') #make the directory for saving sample information, If directory already exist, it would make error 
SampleInfoDir = './SampleInfo/' # save the samples information in Sampleinfo directory, If there isn't SampleInfo directory, occurs error
sample.Initialize(SampleInfoDir = SampleInfoDir) #gices the initial condition to sample

mu = 3.5e-16
gamma = 5e-2
simul = Simulate(sample, dt,mu = mu, gamma = gamma,  eta = 0, LPower = LPower, SampleInfoDir=SampleInfoDir, mul=mul) 


# simul = setOrder(simul, 1, './NOrderParam_0.4grid_180uc_4L_WR_300K_500K_300K_PTO9STO9_gen_3d_74180.txt')

simul.DTp1 = simul.DTp1 * 0

N_iter = 240000 #decides the maximum iterations of calculations
cri = 1e-6 #decides the criteria of calculation limit, if the value of energy 

# free enrgy arrays, which would be filled after calculation, Could use 'append' code, but zero arrays are considered to fast
FE_bulk = np.zeros( N_iter, dtype = np.float128 )
FE_E1 = np.zeros( N_iter, dtype = np.float128 )
FE_E2 = np.zeros( N_iter, dtype = np.float128 )
FE_E3 = np.zeros( N_iter, dtype = np.float128 )
FE_grad = np.zeros( N_iter, dtype = np.float128 )
FE_elec = np.zeros( N_iter, dtype = np.float128 )
FE_sum = np.zeros( N_iter, dtype = np.float128 )

print("Simulation time = ",round(N_iter * dt * 1e12,3), 'ps')

Ex = 0
Ey = 0
Ez = 0
dir = f'0.4grid_{Nx1}uc_{N_L}L_WR_{T0}K_PTO{PTOn}STO{30}_gen_3d_'
os.mkdir(dir)
os.mkdir(f'{dir}/txt')

os.system(f'cp {__file__} {dir}')
os.system(f'cp ./plot_Ngae_hagi.ipynb {dir}')

# 종료 시 마지막 iteration을 저장하는 함수
def save_last_iteration():
    np.savetxt(f'{dir}/txt/NOrderParam_{dir}{i:05d}.txt', np.transpose([simul.P1p0.get(), simul.P2p0.get(), simul.P3p0.get(), simul.P1p1.get(), simul.P2p1.get(), simul.P3p1.get(), 
                                                                        simul.u1p0.get(), simul.u2p0.get(), simul.u3p0.get(),
                                                                        simul.u1p1.get(), simul.u2p1.get(), simul.u3p1.get(),
                                                                        simul.e11p1.get(), simul.e22p1.get(), simul.e33p1.get(), simul.e12p1.get(), simul.e13p1.get(), simul.e23p1.get(),
                                                                        simul.E1.get(), simul.E2.get(), simul.E3.get(), simul.phi.get(),
                                                                        simul.f_bulk.get(), simul.f_E1.get(), simul.f_E2.get(), simul.f_E3.get(), simul.f_gradient.get(),
                                                                        simul.f_electric.get(), simul.f_sum.get()]))

    np.savetxt(f'{dir}/txt/NFreeEnergy_{dir}{i:05d}.txt', np.transpose([FE_bulk, FE_E1, FE_E2, FE_E3, FE_grad, FE_elec, FE_sum]))

    print('Simulation interrupted and data saved.')

# atexit을 사용하여 프로그램 종료 시 save_last_iteration 함수 등록
atexit.register(save_last_iteration)

# SIGTERM 및 SIGINT 신호를 처리하여 save_last_iteration 함수 호출
def signal_handler(signum, frame):
    save_last_iteration()
    os._exit(1)

signal.signal(signal.SIGTERM, signal_handler)
signal.signal(signal.SIGINT, signal_handler)

def get_last_save_point(current_iter, save_period=1000):
    # 마지막 저장 지점을 계산 (1000 단위)
    return (current_iter // save_period) * save_period


# tqdm 출력 결과를 저장할 파일 경로
tqdm_log_file = f"{dir}/tqdm_output.log"


with open(tqdm_log_file, "w") as log_file:
    try:
        for i in tqdm.tqdm ( range( 0, N_iter+1 ) , file=log_file) :

            simul.solveMech(0 , 1/6e-20, 0) # basic, PMLParam
            #simul.solveMech(1 , 1e-6)
            simul.solveElec( Ex, Ey, Ez )
            simul.solveTDGL(0, 5e-2, 0, mul, mean) #basic, PMLParam
            simul.calc_freeE()

            FE_bulk[i] = simul.f_bulk_sum
            FE_E1[i] = simul.f_E1_sum
            FE_E2[i] = simul.f_E2_sum
            FE_E3[i] = simul.f_E3_sum
            FE_grad[i] = simul.f_grad_sum
            FE_elec[i] = simul.f_elec_sum
            FE_sum[i] = simul.f_sum_sum


            save_period = 1000
            if i == 0 or i == save_period:
                np.savetxt(f'{dir}/txt/NOrderParam_{dir}{i:05d}.txt', np.transpose([simul.P1p0.get(), simul.P2p0.get(), simul.P3p0.get(), simul.P1p1.get(), simul.P2p1.get(), simul.P3p1.get(), 
                                                                                    simul.u1p0.get(), simul.u2p0.get(), simul.u3p0.get(),
                                                                                    simul.u1p1.get(), simul.u2p1.get(), simul.u3p1.get(),
                                                                                    simul.e11p1.get(), simul.e22p1.get(), simul.e33p1.get(), simul.e12p1.get(), simul.e13p1.get(), simul.e23p1.get(),
                                                                                    simul.E1.get(), simul.E2.get(), simul.E3.get(), simul.phi.get(),
                                                                                    simul.f_bulk.get(), simul.f_E1.get(), simul.f_E2.get(), simul.f_E3.get(), simul.f_gradient.get(),
                                                                                    simul.f_electric.get(), simul.f_sum.get()]))

                np.savetxt(f'{dir}/txt/NFreeEnergy_{dir}{i:05d}.txt', np.transpose([FE_bulk, FE_E1, FE_E2, FE_E3, FE_grad, FE_elec, FE_sum]))


            if i % save_period == 0 and i != 0 and i != save_period:
                # 이전 파일 삭제
                os.remove(f'{dir}/txt/NOrderParam_{dir}{i-save_period:05d}.txt')
                os.remove(f'{dir}/txt/NFreeEnergy_{dir}{i-save_period:05d}.txt')
                # 현재 파일 저장
                np.savetxt(f'{dir}/txt/NOrderParam_{dir}{i:05d}.txt', np.transpose([simul.P1p0.get(), simul.P2p0.get(), simul.P3p0.get(), simul.P1p1.get(), simul.P2p1.get(), simul.P3p1.get(), 
                                                                                    simul.u1p0.get(), simul.u2p0.get(), simul.u3p0.get(),
                                                                                    simul.u1p1.get(), simul.u2p1.get(), simul.u3p1.get(),
                                                                                    simul.e11p1.get(), simul.e22p1.get(), simul.e33p1.get(), simul.e12p1.get(), simul.e13p1.get(), simul.e23p1.get(),
                                                                                    simul.E1.get(), simul.E2.get(), simul.E3.get(), simul.phi.get(),
                                                                                    simul.f_bulk.get(), simul.f_E1.get(), simul.f_E2.get(), simul.f_E3.get(), simul.f_gradient.get(),
                                                                                    simul.f_electric.get(), simul.f_sum.get()]))
                np.savetxt(f'{dir}/txt/NFreeEnergy_{dir}{i:05d}.txt', np.transpose([FE_bulk, FE_E1, FE_E2, FE_E3, FE_grad, FE_elec, FE_sum]))
            
            
            if  N_iter >= i >= 4000 and Criteria(FE_bulk, FE_E1, FE_E2, FE_E3, FE_sum, i, cri) == True: #criteria
                last_save = get_last_save_point(i)
                os.remove(f'{dir}/txt/NOrderParam_{dir}{last_save:05d}.txt')
                os.remove(f'{dir}/txt/NFreeEnergy_{dir}{last_save:05d}.txt')
                np.savetxt(f'{dir}/txt/NOrderParam_{dir}{i:05d}.txt', np.transpose([simul.P1p0.get(), simul.P2p0.get(), simul.P3p0.get(), simul.P1p1.get(), simul.P2p1.get(), simul.P3p1.get(), 
                                                                                    simul.u1p0.get(), simul.u2p0.get(), simul.u3p0.get(),
                                                                                    simul.u1p1.get(), simul.u2p1.get(), simul.u3p1.get(),
                                                                                    simul.e11p1.get(), simul.e22p1.get(), simul.e33p1.get(), simul.e12p1.get(), simul.e13p1.get(), simul.e23p1.get(),
                                                                                    simul.E1.get(), simul.E2.get(), simul.E3.get(), simul.phi.get(),
                                                                                    simul.f_bulk.get(), simul.f_E1.get(), simul.f_E2.get(), simul.f_E3.get(), simul.f_gradient.get(),
                                                                                    simul.f_electric.get(), simul.f_sum.get()]))

                np.savetxt(f'{dir}/txt/NFreeEnergy_{dir}{i:05d}.txt', np.transpose([FE_bulk, FE_E1, FE_E2, FE_E3, FE_grad, FE_elec, FE_sum]))
            
                
                print('criteria reached')
                bulk_r = np.abs((FE_bulk[i] - FE_bulk[i-1])/FE_bulk[i-1])
                E1_r = np.abs((FE_E1[i] - FE_E1[i-1])/FE_E1[i-1])
                E2_r = np.abs((FE_E2[i] - FE_E2[i-1])/FE_E2[i-1])
                E3_r = np.abs((FE_E3[i] - FE_E3[i-1])/FE_E3[i-1])
                sum_r = np.abs((FE_sum[i] - FE_sum[i-1])/FE_sum[i-1])

                print('bulk_r' , bulk_r)
                print('E1_r' , E1_r)
                print('E2_r' , E2_r)
                print('E3_r' , E3_r)
                print('sum_r' , sum_r)


                break

            if i == N_iter :
                last_save = get_last_save_point(i)
                os.remove(f'{dir}/txt/NOrderParam_{dir}{last_save:05d}.txt')
                os.remove(f'{dir}/txt/NFreeEnergy_{dir}{last_save:05d}.txt')
                np.savetxt(f'{dir}/txt/NOrderParam_{dir}{i:05d}.txt', np.transpose([simul.P1p0.get(), simul.P2p0.get(), simul.P3p0.get(), simul.P1p1.get(), simul.P2p1.get(), simul.P3p1.get(), 
                                                                                    simul.u1p0.get(), simul.u2p0.get(), simul.u3p0.get(),
                                                                                    simul.u1p1.get(), simul.u2p1.get(), simul.u3p1.get(),
                                                                                    simul.e11p1.get(), simul.e22p1.get(), simul.e33p1.get(), simul.e12p1.get(), simul.e13p1.get(), simul.e23p1.get(),
                                                                                    simul.E1.get(), simul.E2.get(), simul.E3.get(), simul.phi.get(),
                                                                                    simul.f_bulk.get(), simul.f_E1.get(), simul.f_E2.get(), simul.f_E3.get(), simul.f_gradient.get(),
                                                                                    simul.f_electric.get(), simul.f_sum.get()]))

                np.savetxt(f'{dir}/txt/NFreeEnergy_{dir}{i:05d}.txt', np.transpose([FE_bulk, FE_E1, FE_E2, FE_E3, FE_grad, FE_elec, FE_sum]))
            
                print('criteria not reached')

    except KeyboardInterrupt:
        save_last_iteration()



# ...여기에 실행할 코드...

finish_time = datetime.datetime.now()
print("simulation started at: ", start_time.strftime("%Y-%m-%d %H:%M:%S"))
print("simulation finished at: ", finish_time.strftime("%Y-%m-%d %H:%M:%S"))

# for i in tqdm.tqdm ( range( 0, N_iter+1 )):

#     simul.solveMech(0 , 1/6e-20, 0) # basic, PMLParam
#     #simul.solveMech(1 , 1e-6)
#     simul.solveElec( Ex, Ey, Ez )
#     simul.solveTDGL(0, 5e-2, 0, mul, mean) #basic, PMLParam
#     simul.calc_freeE()

#     FE_bulk[i] = simul.f_bulk_sum
#     FE_E1[i] = simul.f_E1_sum
#     FE_E2[i] = simul.f_E2_sum
#     FE_E3[i] = simul.f_E3_sum
#     FE_grad[i] = simul.f_grad_sum
#     FE_elec[i] = simul.f_elec_sum
#     FE_sum[i] = simul.f_sum_sum


#     save_period = 2000
#     if i == 0 or i == save_period:
#         np.savetxt(f'{dir}/txt/NOrderParam_{dir}{i:05d}.txt', np.transpose([simul.P1p0.get(), simul.P2p0.get(), simul.P3p0.get(), simul.P1p1.get(), simul.P2p1.get(), simul.P3p1.get(), 
#                                                                             simul.u1p0.get(), simul.u2p0.get(), simul.u3p0.get(),
#                                                                             simul.u1p1.get(), simul.u2p1.get(), simul.u3p1.get(),
#                                                                             simul.e11p1.get(), simul.e22p1.get(), simul.e33p1.get(), simul.e12p1.get(), simul.e13p1.get(), simul.e23p1.get(),
#                                                                             simul.E1.get(), simul.E2.get(), simul.E3.get(), simul.phi.get(),
#                                                                             simul.f_bulk.get(), simul.f_E1.get(), simul.f_E2.get(), simul.f_E3.get(), simul.f_gradient.get(),
#                                                                             simul.f_electric.get(), simul.f_sum.get()]))

#         np.savetxt(f'{dir}/txt/NFreeEnergy_{dir}{i:05d}.txt', np.transpose([FE_bulk, FE_E1, FE_E2, FE_E3, FE_grad, FE_elec, FE_sum]))


#     if i % save_period == 0 and i != 0 and i != save_period:
#         # 이전 파일 삭제
#         os.remove(f'{dir}/txt/NOrderParam_{dir}{i-save_period:05d}.txt')
#         os.remove(f'{dir}/txt/NFreeEnergy_{dir}{i-save_period:05d}.txt')
#         # 현재 파일 저장
#         np.savetxt(f'{dir}/txt/NOrderParam_{dir}{i:05d}.txt', np.transpose([simul.P1p0.get(), simul.P2p0.get(), simul.P3p0.get(), simul.P1p1.get(), simul.P2p1.get(), simul.P3p1.get(), 
#                                                                             simul.u1p0.get(), simul.u2p0.get(), simul.u3p0.get(),
#                                                                             simul.u1p1.get(), simul.u2p1.get(), simul.u3p1.get(),
#                                                                             simul.e11p1.get(), simul.e22p1.get(), simul.e33p1.get(), simul.e12p1.get(), simul.e13p1.get(), simul.e23p1.get(),
#                                                                             simul.E1.get(), simul.E2.get(), simul.E3.get(), simul.phi.get(),
#                                                                             simul.f_bulk.get(), simul.f_E1.get(), simul.f_E2.get(), simul.f_E3.get(), simul.f_gradient.get(),
#                                                                             simul.f_electric.get(), simul.f_sum.get()]))
#         np.savetxt(f'{dir}/txt/NFreeEnergy_{dir}{i:05d}.txt', np.transpose([FE_bulk, FE_E1, FE_E2, FE_E3, FE_grad, FE_elec, FE_sum]))
    
    
#     if  N_iter >= i >= 20000 and Criteria(FE_bulk, FE_E1, FE_E2, FE_E3, FE_sum, i, cri) == True: #criteria
#         np.savetxt(f'{dir}/txt/NOrderParam_{dir}{i:05d}.txt', np.transpose([simul.P1p0.get(), simul.P2p0.get(), simul.P3p0.get(), simul.P1p1.get(), simul.P2p1.get(), simul.P3p1.get(), 
#                                                                             simul.u1p0.get(), simul.u2p0.get(), simul.u3p0.get(),
#                                                                             simul.u1p1.get(), simul.u2p1.get(), simul.u3p1.get(),
#                                                                             simul.e11p1.get(), simul.e22p1.get(), simul.e33p1.get(), simul.e12p1.get(), simul.e13p1.get(), simul.e23p1.get(),
#                                                                             simul.E1.get(), simul.E2.get(), simul.E3.get(), simul.phi.get(),
#                                                                             simul.f_bulk.get(), simul.f_E1.get(), simul.f_E2.get(), simul.f_E3.get(), simul.f_gradient.get(),
#                                                                             simul.f_electric.get(), simul.f_sum.get()]))

#         np.savetxt(f'{dir}/txt/NFreeEnergy_{dir}{i:05d}.txt', np.transpose([FE_bulk, FE_E1, FE_E2, FE_E3, FE_grad, FE_elec, FE_sum]))
    
        
#         print('criteria reached')
#         bulk_r = np.abs((FE_bulk[i] - FE_bulk[i-1])/FE_bulk[i-1])
#         E1_r = np.abs((FE_E1[i] - FE_E1[i-1])/FE_E1[i-1])
#         E2_r = np.abs((FE_E2[i] - FE_E2[i-1])/FE_E2[i-1])
#         E3_r = np.abs((FE_E3[i] - FE_E3[i-1])/FE_E3[i-1])
#         sum_r = np.abs((FE_sum[i] - FE_sum[i-1])/FE_sum[i-1])

#         print('bulk_r' , bulk_r)
#         print('E1_r' , E1_r)
#         print('E2_r' , E2_r)
#         print('E3_r' , E3_r)
#         print('sum_r' , sum_r)


#         break

#     if i == N_iter :
#         np.savetxt(f'{dir}/txt/NOrderParam_{dir}{i:05d}.txt', np.transpose([simul.P1p0.get(), simul.P2p0.get(), simul.P3p0.get(), simul.P1p1.get(), simul.P2p1.get(), simul.P3p1.get(), 
#                                                                             simul.u1p0.get(), simul.u2p0.get(), simul.u3p0.get(),
#                                                                             simul.u1p1.get(), simul.u2p1.get(), simul.u3p1.get(),
#                                                                             simul.e11p1.get(), simul.e22p1.get(), simul.e33p1.get(), simul.e12p1.get(), simul.e13p1.get(), simul.e23p1.get(),
#                                                                             simul.E1.get(), simul.E2.get(), simul.E3.get(), simul.phi.get(),
#                                                                             simul.f_bulk.get(), simul.f_E1.get(), simul.f_E2.get(), simul.f_E3.get(), simul.f_gradient.get(),
#                                                                             simul.f_electric.get(), simul.f_sum.get()]))

#         np.savetxt(f'{dir}/txt/NFreeEnergy_{dir}{i:05d}.txt', np.transpose([FE_bulk, FE_E1, FE_E2, FE_E3, FE_grad, FE_elec, FE_sum]))
    
#         print('criteria not reached')
