//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

channel C extends ned.DatarateChannel{
    datarate = default(100Mbps);
}
simple Txc17{
    parameters:
        volatile double srp = default(uniform(0,1));
    gates:
        input in[];
        output out[];
}
network P{
    parameters:
        int c = default(6);
        volatile double sendInterval @unit(s) = default(exponential(1s));
    submodules:
        G: RandomGraph{
            count = c;
            connectedness = 1.0;
        }   
}
module RandomGraph {
    parameters:
        int count;
        double connectedness; // 0.0<x<1.0
    submodules:
        node[count]: Txc17 {
            gates:
                in[count];
                out[count];
        }
    connections allowunconnected:
        for i=0..count-1, for j=0..count-1{
            	node[i].out[j] --> C --> node[j].in[i]
                if i!=j && uniform(0,1)<connectedness;
        }
}