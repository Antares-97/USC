import React from 'react';
import Badge from 'react-bootstrap/Badge'
import '../App.css';

function tagBadge(props){
    if(props.tag==="world"){
        return <Badge className="badge-world" style={props.style}>WORLD</Badge>;
    }
    if(props.tag==="politics"){
        return <Badge className="badge-politics" style={props.style}>POLITICS</Badge>;
    }
    if(props.tag==="business"){
        return <Badge className="badge-business" style={props.style}>BUSINESS</Badge>;
    }
    if(props.tag==="technology"){
        return <Badge className="badge-technology" style={props.style}>TECHNOLOGY</Badge>;
    }
    if(props.tag==="sports"||props.tag==="sport"){
        return <Badge className="badge-sports" style={props.style}>SPORTS</Badge>;
    }
    if(props.tag==="GUARDIAN"){
        return <Badge className="badge-guardian" style={props.style}>GUARDIAN</Badge>;
    }
    if(props.tag==="NYTIMES"){
        return <Badge className="badge-nytimes" style={props.style}>NYTIMES</Badge>;
    }
    else{
        const str = props.tag;
        return <Badge className="badge-other" style={props.style}>{str.toUpperCase()}</Badge>;
    }
}

export default tagBadge;