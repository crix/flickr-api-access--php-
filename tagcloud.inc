<?php 

// This mostly from http://www.joshgerdes.com/projects/flickr-tag-cloud-widget/

error_reporting( E_ALL ); // remove this in production!

function file_get_contents_curl($url) {
  $ch = curl_init();
  curl_setopt($ch, CURLOPT_HEADER, 0);
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt($ch, CURLOPT_URL, $url);
  $data = curl_exec($ch);
  curl_close($ch);
  return $data;
}

// data retrieval function
function get() {
    
  $title = 'Tags';
  $flickrid = 'YOUR ID HERE';
  $minfontsize = '12';
  $maxfontsize = '35';
  $api_key= 'YOUR API KEY HERE';
  $maxcount = '1000';
  $cache = 'cache';
  $target = 'target';
  
  // Setup properties for API call    
  $params = array(
    'api_key' => $api_key,
    'method'  => 'flickr.people.getInfo',
    'user_id' => $flickrid,
    'format'  => 'php_serial',
  );
  
  // Loop through parameters and encode
  $encoded_params = array();
  foreach ($params as $k => $v){
    // Encode parameters for the url of the API call
    $encoded_params[] = urlencode($k).'='.urlencode($v);
  }
  
  // Call the API and decode the response
  $url = "http://api.flickr.com/services/rest/?".implode('&', $encoded_params);
  
  // Fetch the info
  $rsp = file_get_contents_curl($url);
  $rsp_obj = unserialize($rsp);
  
  // Check if successful and display error if not
  if ($rsp_obj['stat'] == 'ok'){
    // Grab the photo url for the given person
    $photourl = $rsp_obj['person']['photosurl']['_content'];
  }
  
  // Setup properties for API call    
  $params = array(
    'api_key' => $api_key,
    'method'  => 'flickr.tags.getListUserPopular',
    'count'   => $maxcount,
    'user_id' => $flickrid,
    'format'  => 'php_serial',
  );
  
  $encoded_params = array();
  // Loop through parameters and encode
  foreach ($params as $k => $v){
    // Encode parameters for the url of the API call
    $encoded_params[] = urlencode($k).'='.urlencode($v);
  }

  // Call the API and decode the response
  $url = "http://api.flickr.com/services/rest/?".implode('&', $encoded_params);   

  // Fetch the info
  $rsp = file_get_contents_curl($url);
  $rsp_obj = unserialize($rsp);

  $data = "\n";
  // Check if successful and display error if not
  if ($rsp_obj['stat'] == 'ok'){
  
    // Reset the local count
    $count = 0; 
    $mintagcount = 0;
    $maxtagcount = 0;
    $tags = Array();
    
    // Loop through tags
    foreach( $rsp_obj['who']['tags']['tag'] as $tag )
    {
      if($count==0){
        $mintagcount=$tag['count'];
        $maxtagcount=$tag['count'];         
      }
      
      if($mintagcount>$tag['count'])
        $mintagcount=$tag['count'];
        
      if($maxtagcount<$tag['count'])
        $maxtagcount=$tag['count']; 
                
      $aux[0]=$tag['count'];
      $aux[1]=$tag['_content'];
      $tags[$count]=$aux;
      
      $count++;
    }
    
    if($maxtagcount==$mintagcount)
      $mintagcount=$maxtagcount-1;

    foreach($tags as $tag){
      $not_allowed = array('auto'); // Remove tags you dont want
      if(in_array($tag[1], $not_allowed)) {
        $fontsize='0';        
      }
      else {
        $fontsize=round(((($maxfontsize-$minfontsize)/($maxtagcount-$mintagcount))*($tag[0]-$mintagcount))+$minfontsize);
      }

      // Write out link tag cloud style. 
      $data .= "<a href=\"" . $photourl ."tags/".$tag[1]."\" target=\"" .$target. "\"  rel=\"nofollow\" style=\"font-size: ".$fontsize."px;\">".$tag[1]."</a>&nbsp;\n";
    }
  }
  else {
    // Check if error code given and display
    if ( $rsp_obj['stat'] == 'fail' ) {
      $data .= "Error code: ".$rsp_obj['code']." ".$rsp_obj['message']."<br />\n";
    }
    else {
      $data .= "Unable to connect to flickr";
    }
  }
  return $data;
}
echo get()
 ?>
 