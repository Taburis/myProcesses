
from ROOT import TFile, TH1, TH1D
import decimal

class hepDataWriter : 
    def __init__(self):
        self.s = ''
        self.qualifier = {}
        self.range = []
        self.made_dep = 0
        self.nsig = 2 

    def addQualifier(self, key, value):
        self.qualifier[key] = value

    def findDecimal(self, num):
        d = decimal.Decimal(str(num))
        return abs(d.as_tuple().exponent)

    def significantDecimal(self, num1, nsig):
        n = self.findDecimal(num1)
        n0 = 0
        for s in str(num1):
            if s !='0' and s!='.': n0 +=1
        return n+nsig-n0

    def consistDecimal(self, value, err1, err2):
        errr1 = format(err1, '.'+str(self.nsig)+'g')  
        errr2 = format(err2, '.'+str(self.nsig)+'g')  
        n1 = self.significantDecimal(errr1, self.nsig)
        n2 = self.significantDecimal(errr2, self.nsig)
        nde= 0
        if   n1>=n2 : nde = n1 
        elif n1<n2 : nde = n2
        #print(nde)
        ndigi = '.'+str(nde)+'f'
        return format(value,ndigi), format(err1, ndigi), format(err2, ndigi)

    def makeHeader(self, header):
        self.s+= "- header: {name: '"+header+"'}\n"
        self.s+= "  qualifiers:\n"
        self.s+= "  - {name: SQRT(S)/NUCLEON, units: GEV, value: 5020.0}\n"
        for key, value in self.qualifier.items():
            self.s += '  - {name: '+key+', value: '+value+'}\n'
        self.qualifier.clear()

    def add_Y_dependentVar(self, h, hsys, header):
        if self.made_dep == 0 : 
            self.s += "dependent_variables:\n"
            self.made_dep =1
        self.makeHeader(header)
        self.s += "  values:\n"
        nxbin = h.GetNbinsX()
        for i in range(1,nxbin+1):
            if hsys.GetBinCenter(i) > self.range[1] or hsys.GetBinCenter(i) < self.range[0] : continue
            error = h.GetBinError(i)
            sys   = hsys.GetBinError(i)
            value = h.GetBinContent(i)
            value, sys, error = self.consistDecimal(value, sys, error)
            self.s += "  - errors:\n"
            self.s += "    - {label: stat, symerror: "+str(error)+"}\n"
            self.s += "    - {label: sys,  symerror: "+str(sys)+"}\n"
            self.s += "    value: "+str(value)+"\n"
    def add_X_independentVar(self, h, header):
        self.s += "independent_variables:\n"
        incen = " "
        self.s += "- header: {name: '"+header+"'}\n  values:\n"
        incen += " "
        nxbin = h.GetNbinsX()
        for i in range(1,nxbin+1):
            ax = h.GetXaxis()
            low = format(ax.GetBinLowEdge(i),'.'+str(self.nsig)+'g')
            high  = format(ax.GetBinUpEdge(i),'.'+str(self.nsig)+'g')
            if h.GetBinCenter(i) > self.range[1] or h.GetBinCenter(i) < self.range[0] : continue
            self.s += incen+"- {high: "+str(high)+", low: "+str(low)+"}\n"

    def dump_file(self, path):
        with open(path, 'w') as ff:
            ff.write(self.s)
            ff.close()
        print('string dumpped to: ' + path)
        self.s=''
        self.qualifier = {}
        self.made_dep = 0

if __name__ == "__main__":
    path = '/Users/tabris/frameLite/hin18020/plots/AN17337Result_new.root'
    inf = TFile.Open(path)
    wr = hepDataWriter()
    ylabel = "$Y=\\frac{1}{N_{\\text{jet}}}\\frac{dN}{d\Delta r}$"
    wr.range = [0, 1.]
    for i in range(5):
        h = inf.Get("dr_signal_dijtc_yield_step3_"+str(i)+"_0")
        hsys = inf.Get("dr_dijtc_yield_systError_"+str(i)+"_0")
        wr.addQualifier('RE', 'P P --> CHARGED X')
        wr.add_Y_dependentVar(h, hsys, ylabel)
    wr.add_X_independentVar(h, '$\Delta r$')
    wr.dump_file('./data1.yaml')
    #wr.dump_file('./yield_deta.yaml')
"""
    ylabel = "$Y_{\\text{PbPb}} - Y_{\\text{pp}}$"
    for i in range(4):
         h = inf.Get("py_deta_diff_"+str(i))
         hsys = inf.Get("py_deta_diff_syst_"+str(i))
         wr.addQualifier('RE', 'PB PB --> CHARGED X')
         wr.addQualifier('CENTRALITY', cent_label[i])
         wr.add_Y_dependentVar(h, hsys, ylabel)
    wr.add_X_independentVar(hsys, '$\Delta\eta$')
    wr.dump_file('./data2.yaml')
    #wr.dump_file('./yield_diff_deta.yaml')
    inf.Close()

    inf = TFile.Open('/Users/tabris/cmsProjects/inclusiveJetTrackCorrelation2015/macros/plots/officialHist_py_dphi_16_020.root')
    wr = hepDataWriter()
    cent_label = ['0-10%', '10-30%', '30-50%', '50-100%', 'pp']
    ylabel = "$Y=\\frac{1}{N_{\\text{jet}}}\\frac{dN}{d\Delta\phi}$"
    wr.range = [-1.5, 1.5]
    for i in range(5):
        h = inf.Get("py_dphi_all_"+str(i))
        hsys = inf.Get("py_dphi_err_all_"+str(i))
        if i < 4 : wr.addQualifier('RE', 'PB PB --> CHARGED X')
        else : wr.addQualifier('RE', 'P P --> CHARGED X')
        #wr.addQualifier('RE', 'PB PB --> CHARGED X')
        wr.addQualifier('CENTRALITY', cent_label[i])
        wr.add_Y_dependentVar(h, hsys, ylabel)
    wr.add_X_independentVar(h, '$\Delta\phi$')
    #wr.dump_file('./yield_dphi.yaml')
    wr.dump_file('./data3.yaml')

    ylabel = "$Y_{\\text{PbPb}} - Y_{\\text{pp}}$"
    for i in range(4):
         h = inf.Get("py_dphi_diff_"+str(i))
         hsys = inf.Get("py_dphi_diff_syst_"+str(i))
         wr.addQualifier('RE', 'PB PB --> CHARGED X')
         wr.addQualifier('CENTRALITY', cent_label[i])
         wr.add_Y_dependentVar(h, hsys, ylabel)
    wr.add_X_independentVar(hsys, '$\Delta\phi$')
    #wr.dump_file('./yield_diff_dphi.yaml')
    wr.dump_file('./data4.yaml')
    inf.Close()

    inf = TFile.Open('/Users/tabris/cmsProjects/inclusiveJetTrackCorrelation2015/macros/plots/JS5TeV_HIN_16_020.root')
    wr = hepDataWriter()
    cent_label = ['0-10%', '10-30%', '30-50%', '50-100%', 'pp']
    ylabel = "$P(\Delta r)$"
    wr.range = [0, 1]
    for i in range(5):
        h = inf.Get("st_"+str(i)+"_sum")
        hsys = inf.Get("JS_dr_err_all_"+str(i))
        #wr.addQualifier('RE', 'PB PB --> CHARGED X')
        if i < 4 : wr.addQualifier('RE', 'PB PB --> CHARGED X')
        else : wr.addQualifier('RE', 'P P --> CHARGED X')
        wr.addQualifier('CENTRALITY', cent_label[i])
        wr.add_Y_dependentVar(h, hsys, ylabel)
    wr.add_X_independentVar(h, '$\Delta r$')
#    wr.dump_file('./Rho_dr.yaml')
    wr.dump_file('./data9.yaml')

    pt_label = ['0.7', '2', '4']
    ylabel = "$P(\Delta r)_{\\text{PbPb}}/P(\Delta r)_{\\text{pp}}$"
    for i in range(4):
        for j in range(3):
            h = inf.Get("sub_ratio_"+str(j)+'_'+str(i)+'_auxi_band')
            hsys = inf.Get("sub_ratio2_"+str(j)+'_'+str(i))
            wr.addQualifier('RE', 'PB PB --> CHARGED X')
            wr.addQualifier('CENTRALITY', cent_label[i])
            wr.addQualifier('LOWER TRK PT CUT, units: GEV', pt_label[j])
            wr.add_Y_dependentVar(h, hsys, ylabel)
    wr.add_X_independentVar(hsys, '$\Delta r$')
#    wr.dump_file('./Rho_ratio_dr.yaml')
    wr.dump_file('./data10.yaml')
    inf.Close()

    inf = TFile.Open('/Users/tabris/cmsProjects/inclusiveJetTrackCorrelation2015/macros/plots/JS5TeV_normalized_HIN_16_020.root')
    cent_label = ['0-10%', '10-30%', '30-50%', '50-100%', 'pp']
    ylabel = "$\\rho(\Delta r)$"
    wr.range = [0, 1]
    for i in range(5):
        h = inf.Get("st_"+str(i)+"_sum")
        hsys = inf.Get("JS_dr_err_all_"+str(i))
        #wr.addQualifier('RE', 'PB PB --> CHARGED X')
        if i < 4 : wr.addQualifier('RE', 'PB PB --> CHARGED X')
        else : wr.addQualifier('RE', 'P P --> CHARGED X')
        wr.addQualifier('CENTRALITY', cent_label[i])
        wr.add_Y_dependentVar(h, hsys, ylabel)
    wr.add_X_independentVar(h, '$\Delta r$')
    #wr.dump_file('./js_normalized_dr.yaml')
    wr.dump_file('./data11.yaml')

    wr.range = [0, 1]
    ylabel = "$\\rho(\Delta r)_{\\text{PbPb}}/\\rho(\Delta r)_{\\text{pp}}$"
    for i in range(4):
        h = inf.Get("js_ratio_"+str(i))
        hsys = inf.Get("js_syst_ratio_"+str(i))
        wr.addQualifier('RE', 'PB PB --> CHARGED X')
        wr.addQualifier('CENTRALITY', cent_label[i])
        wr.add_Y_dependentVar(h, hsys, ylabel)
    wr.add_X_independentVar(h, '$\Delta r$')
    #wr.dump_file('./js_ratio_normalized_dr.yaml')
    wr.dump_file('./data12.yaml')
    inf.Close()

    inf = TFile.Open('/Users/tabris/cmsProjects/inclusiveJetTrackCorrelation2015/macros/plots/officialHist_py_dr_16_020.root')
    cent_label = ['0-10%', '10-30%', '30-50%', '50-100%', 'pp']
    ylabel = "$Y=\\frac{1}{N_{\\text{jet}}}\\frac{dN}{d\\Delta r}$"
    wr.range = [0, 1]
    for i in range(5):
        h = inf.Get("py_dr_all_"+str(i))
        hsys = inf.Get("py_dr_err_all_"+str(i))
        if i < 4 : wr.addQualifier('RE', 'PB PB --> CHARGED X')
        else : wr.addQualifier('RE', 'P P --> CHARGED X')
        wr.addQualifier('CENTRALITY', cent_label[i])
        wr.add_Y_dependentVar(h, hsys, ylabel)
    wr.add_X_independentVar(h, '$\Delta r$')
    #wr.dump_file('./yield_dr.yaml')
    wr.dump_file('./data5.yaml')

    ylabel = "$Y_{\\text{PbPb}} - Y_{\\text{pp}}$"
    for i in range(4):
        h = inf.Get("py_dr_diff_"+str(i))
        hsys = inf.Get("py_dr_diff_syst_"+str(i))
        wr.addQualifier('RE', 'PB PB --> CHARGED X')
        wr.addQualifier('CENTRALITY', cent_label[i])
        wr.add_Y_dependentVar(h, hsys, ylabel)
    wr.add_X_independentVar(h, '$\Delta r$')
    #wr.dump_file('./yield_diff_dr.yaml')
    wr.dump_file('./data6.yaml')
    inf.Close()

    inf = TFile.Open('/Users/tabris/cmsProjects/inclusiveJetTrackCorrelation2015/macros/plots/officialHist_py_integrated_16_020.root')
    wr.range = [0.7, 20]
    ylabel = "$\\frac{dN}{dp_{\\text{T}}}$"
    for i in range(4):
        h = inf.Get("pb_yield_"+str(i))
        hsys = inf.Get("pb_yield_err_"+str(i))
        wr.addQualifier('RE', 'PB PB --> CHARGED X')
        wr.addQualifier('CENTRALITY', cent_label[i])
        wr.add_Y_dependentVar(h, hsys, ylabel)
    h = inf.Get("pp_yield")
    hsys = inf.Get("pp_yield_err")
    wr.addQualifier('RE', 'P P --> CHARGED X')
    wr.addQualifier('CENTRALITY', 'pp')
    wr.add_Y_dependentVar(h, hsys, ylabel)
    wr.add_X_independentVar(h, '$p_{\\text{T}}^{\\text{trk}}$')
    #wr.dump_file('./yield_integrated.yaml')
    wr.dump_file('./data7.yaml')

    ylabel = "$\\frac{dN_{\\text{PbPb}}}{dp_{\\text{T}}}-\\frac{dN_{\\text{pp}}{dp_{\\text{T}}}$"
    for i in range(4):
        h = inf.Get("yield_diff_"+str(i))
        hsys = inf.Get("yield_diff_err_"+str(i))
        wr.addQualifier('RE', 'PB PB --> CHARGED X')
        wr.addQualifier('CENTRALITY', cent_label[i])
        wr.add_Y_dependentVar(h, hsys, ylabel)
    wr.add_X_independentVar(h, '$p_{\\text{T}}^{\\text{trk}}$')
    #wr.dump_file('./yield_diff_integrated.yaml')
    wr.dump_file('./data8.yaml')
"""
    #inf.Close()
