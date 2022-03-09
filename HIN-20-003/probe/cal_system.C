

void cal_system(){
	vector<float> syst;
	syst.push_back(0.03);
	syst.push_back(0.058);
	syst.push_back(0.05);
	syst.push_back(0.08);
	syst.push_back(0.042);
	syst.push_back(0.02);
	syst.push_back(0.01);

	double res = 0;	
	for(auto &it: syst){
		res+= pow(it, 2);
	}
	cout<<sqrt(res)<<endl;
}
