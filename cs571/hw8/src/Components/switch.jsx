import React from 'react';
import Switch from 'react-switch';

function SwitchToggle(props){
    return (
        <div className="react-switch-Switch" style={props.style}>
            NYTimes
            <Switch 
                onChange={props.onClick} 
                checked={props.isOn} 
                offColor={'#e8e7e3'}
                onColor={'#41a1fa'}
                uncheckedIcon={false}
                checkedIcon={false}
                className="react-switch-toggle"
            /> 
            Guardian
        </div>
    );
}
 
export default SwitchToggle;