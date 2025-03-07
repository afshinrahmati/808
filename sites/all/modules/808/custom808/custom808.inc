<?php

/*------------------api version code admin menu-------------------*/
function api_version_lists(){
    $query = db_select("api_code_version" , "version")
        ->fields("version")->execute()->fetchAll();

    foreach ($query as $data) {
        $rows[] = array(
            array('data' => $data->url),
            array('data' => $data->method),
            array('data' => $data->source_name),
            array('data' => $data->version),
            array('data' => $data->last_function),
            array('data' => l(t('Edit'), 'admin/structure/services/apiversion/edit/' . $data->id)  . ' - ' . l(t('Delete'), 'admin/structure/services/apiversion/delete/' . $data->id)),
        );
    }

    return theme('table', array('header'=>array(t('url'), t('method'),t('source'),t('version'), t('last_function'), t('action')), 'rows'=>$rows));
}
function add_api_version_form($form, &$form_state , $id = 0){
    try {
        $data = (array) db_select("api_code_version" , "version")
            ->fields("version")->condition("id" , $id)->execute()->fetch();

        $form['id'] = array('#type' => 'hidden', '#value' => $id);

        $form['url'] = array(
            '#type' => 'textfield',
            '#title' => t('مسیر api'),
            '#description' => t('مسیر api را وارد کنید'),
            '#default_value' => (!empty($data["url"]))? $data["url"] : '',
        );
        $form['method'] = array(
            '#type' => 'textfield',
            '#title' => t('method'),
            '#description' => t('متد ریکوئست'),
            '#default_value' => (!empty($data["method"]))? $data["method"] : '',
        );
        $form['source'] = array(
            '#type' => 'textfield',
            '#title' => t('نوع فرستنده'),
            '#description' => t('نوع فرستنده درخواست، android , ios , ...'),
            '#default_value' => (!empty($data["source_name"]))? $data["source_name"] : '',
        );
        $form['version'] = array(
            '#type' => 'textfield',
            '#title' => t('ورژن'),
            '#description' => t('ورژن فرستنده کد'),
            '#default_value' => (isset($data["version"]))? $data["version"] : '',
        );
        $form['last_function'] = array(
            '#type' => 'textfield',
            '#title' => t('آخرین تابعی'),
            '#description' => t('نام دقیق آخرین تابعی که در این ورژن استفاده می شود.'),
            '#default_value' => (!empty($data["last_function"]))? $data["last_function"] : '',
        );
        $form['description'] = array(
            '#type' => 'textarea',
            '#title' => t('توضیحات'),
            '#description' => t('توضیح برای داکیمومنت کردن'),
            '#default_value' => (!empty($data["description"]))? $data["description"] : '',
        );

        $form['submit'] = array(
            '#type' => 'submit',
            '#value' => t('Save')
        );

        return $form;
    } catch (Exception $e) {
        echo 'Caught exception: ',  $e->getMessage(), "\n";
    }
}
function add_api_version_form_validate($form, &$form_state){
    try{
        if(empty($form_state['values']['url']) && strlen($form_state['values']['url']) == 0) form_set_error('url', t('مسیر اجباری است'));
        if(empty($form_state['values']['method']) && strlen($form_state['values']['method']) == 0) form_set_error('method', t('متود اجباری است'));
        if(empty($form_state['values']['source']) && strlen($form_state['values']['source']) == 0) form_set_error('source', t('نام منبع اجباری است'));
        if(empty($form_state['values']['version']) && strlen($form_state['values']['version']) == 0) form_set_error('version', t('ورژن اجباری است'));
        if(empty($form_state['values']['last_function']) && strlen($form_state['values']['last_function']) == 0) form_set_error('last_function', t('نام تابع اجباری است'));
    }
    catch (Exception $e) {
        drupal_set_message($e->getMessage() , 'error');
        watchdog('Submit api code version failed', $e->getMessage(), WATCHDOG_CRITICAL);
    }
}
function add_api_version_form_submit($form, &$form_state){
    try{
        db_merge('api_code_version')
            ->key(array('id' => $form_state["values"]["id"]))
            ->fields(array(
                'url' => $form_state["values"]["url"] ,
                'method' => $form_state["values"]["method"] ,
                'source_name' => $form_state["values"]["source"] ,
                'version' => $form_state["values"]["version"] ,
                'last_function' => $form_state["values"]["last_function"] ,
                'description' => $form_state["values"]["description"] ,
                'created' => time()
            ))
            ->execute();

        drupal_set_message('تغییرات با موفقیت ذخیره شد.');
    } catch (Exception $e) {
        drupal_set_message($e->getMessage() , 'error');
        watchdog('Submit api code version failed', $e->getMessage(), WATCHDOG_CRITICAL);
    }
}
function api_version_delete($id){

    $deleted = db_delete('api_code_version')
        ->condition('id', $id)
        ->execute();

    drupal_goto('admin/structure/services/apiversion');
}
/*----------------------------------------------------------------*/
/*----------------------push notification-------------------------*/
function push_notification_form($form, &$form_state){
    try {
        $form['title'] = array(
            '#type' => 'textfield',
            '#title' => t('عنوان نوتیفیکیشن'),
            '#required' => true,
        );
        $form['message'] = array(
            '#type' => 'textarea',
            '#title' => t('پیام نوتیفیکیشن'),
            '#required' => true,
        );
        $form['type'] = array(
            '#type' => 'select',
            '#title' => t('نوع محتوا را مشخص کنید'),
            '#options' => array("main" => "سایر" , "publication" => "ایبوک" , "college" => "دوره" , "questions" => "پرسش و پاسخ" , "education" => "ورکشاپ" , "product_kit" => "پکیج" , "link" => "لینک"),
        );
        $form['nid'] = array(
            '#type' => 'textfield',
            '#title' => t('شماره نود یا لینک'),
        );
        $form['playsound'] = array(
            '#type' => 'select',
            '#title' => t('صدا داشته باشد یا خیر'),
            '#options' => array("true" => "بله", "false" => "خیر"),
        );
        $form['file'] = array(
            '#type' => 'file',
            '#title' => t('انتخاب عکس'),
            '#description' => t('فقط از فرمت های jpg jpeg png استفاده کنید.'),
        );

        $query = db_select("role" , "role");
        $query->fields("role" , array("rid" , "name"));
        $query->condition("role.rid" , array(1 , 2 , 13) , "NOT IN");
        $db_roles = $query->execute()->fetchAll();
        foreach ($db_roles as $db_role){
            $roles[$db_role->rid] = $db_role->name;
        }
        ksort($roles);
        $form['user'] = array(
            '#type' => 'checkboxes',
            '#title' => t('به چه کاربرانی ارسال گردد؟'),
            '#options' => $roles,
            '#description' => t('در صورتی که نقشی انتخاب نشود به همه کاربران ارسال می شود.'),
        );

        $form['submit'] = array(
            '#type' => 'submit',
            '#value' => "ارسال"
        );

        return $form;
    } catch (Exception $e) {
        echo 'Caught exception: ',  $e->getMessage(), "\n";
    }
}
function push_notification_form_validate($form, &$form_state){
    if(strcmp($form_state["values"]["type"] , "main") == 0) {
        $form_state["values"]["type"] = "";
        $form_state["values"]["nid"] = "";
    }elseif(strcmp($form_state["values"]["type"] , "link") == 0){
        if(empty($form_state["values"]["nid"]) && strlen($form_state['values']['nid']) == 0)
            form_set_error('nid', t('فیلد لینک اجباری است.'));
    }
    else{
        if(empty($form_state["values"]["nid"]) && strlen($form_state['values']['nid']) == 0) form_set_error('nid', t('انتخاب شماره نود برای نوع محتوااجباری است.'));
        $query = db_select("node" , "node")->fields("node" , array("nid"))->condition("nid" , $form_state["values"]["nid"])->condition("type" , $form_state["values"]["type"])->execute()->fetchAll();
        if(empty($query)) form_set_error('nid', t('شماره نود با نوع محتوا مطابقت ندارد'));
    }
}
function push_notification_form_submit($form, &$form_state){
    $data = array();

    $file = file_save_upload('file', array(), 'public://');
    if (!empty($file->fid)) {
        $file->status = FILE_STATUS_PERMANENT;
        file_save($file);
        file_usage_add($file, 'custom808', 'files', $file->fid);
    }

    $data["title"] = $form_state["values"]["title"];
    $data["message"] = $form_state["values"]["message"];
    $data["type"] = $form_state["values"]["type"];
    $data["nid"] = $form_state["values"]["nid"];
    $data["playsound"] = $form_state["values"]["playsound"];
    $data["image"] = (!empty($file)) ? image_style_url("200x200" , $file->uri) : "";
    $data["timestamp"] = REQUEST_TIME;

    $roles = array();
    foreach ($form_state["values"]["user"] as $item) {
        if($item) array_push($roles , $item);
    }
    $query = db_select("app_notification_token" , "token");
    $query->fields("token" , array("token"));
    if(!empty($roles)){
        $query->join("users_roles" , "role" , "role.uid = token.uid");
        $query->condition("role.rid" , $roles , "IN");
    }
    $tokens = $query->execute()->fetchCol();

    $service_url = "https://fcm.googleapis.com/fcm/send";
    $post_data = array("data" => $data ,
        "registration_ids" => $tokens
    );
    $post_data = json_encode($post_data);
    $curl = curl_init($service_url);
    curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($curl, CURLOPT_POST, true);
    curl_setopt($curl, CURLOPT_POSTFIELDS, $post_data);
    curl_setopt($curl, CURLOPT_HTTPHEADER, array(
        'Content-Type:application/json' ,
        'Authorization:key=AAAALnpKK78:APA91bGyZg-Gj_J0luz_4E4ZOXApm6SRmOuwYz6M5ZeOXJ-EoPNE7cAdhyV6tqSeET6zQSltiGBQvMuLTB_RvtD7yA4Z1n59Dn0leBHu3XeTSvHHmT-NEUmDjENaJX_MMjDArwfvFNqD'
        )
    );
    curl_setopt($curl, CURLOPT_VERBOSE, true);
    $response = curl_exec($curl);
    curl_close($curl);
    $response = json_decode($response);

    global $user;
    db_insert("push_notification_info")->fields(array(
        "uid" => $user->uid,
        "title" => $data["title"],
        "message" => $data["message"],
        "content_type" => $data["type"],
        "nid" => $data["type"] != "link" ? $data["nid"] : 0,
        "fid" => !empty($file->fid) ? $file->fid : 0,
        "picture" => $data["image"],
        "receivers" => !empty($roles) ? implode(" , " , $roles) : "",
        "success" => $response->success,
        "failure" => $response->failure,
        "time" => REQUEST_TIME,
    ))->execute();

    $message = "اعلان های درست : " . $response->success . " <br> " . "اعلان های نادرست : " . $response->failure;
    drupal_set_message($message , "status");
}

/*table to show the previous sent notification*/
function sent_notification_table_form($form, &$form_state){
    try {
        /*fetch the list of actions for filter the actions*/
        $query = db_select("push_notification_info" , "notif_info");
        $query->fields("notif_info" , array("id" , "title" , "message" , "content_type" , "nid" , "picture" , "success" , "failure" , "time"));
        $query->orderBy("notif_info.id" , "DESC");
        $result = $query->execute()->fetchAll();

        /*render the table log*/
        $header = array("id" , "title" , "message" , "content_type" , "nid" , "picture" , "success" , "failure" , "time");
        $rows = array();
        foreach ($result as $key=>$row) {
            $row->time = format_date($row->time);
            $row->picture = '<img src="' . $row->picture . '" height=50px; width=50px>';

            /*convert object to array for being able to add row number at the first column*/
            $row = (array)$row;
            $rows[] = array('data' => $row);
        }
        $form['table'] = array(
//            '#markup' => theme('table', array('header' => $header, 'rows' => $rows)) . theme('pager'),
            '#markup' => theme('table', array('header' => $header, 'rows' => $rows)),
        );

        return $form;
    } catch (Exception $e) {
        echo 'Caught exception: ',  $e->getMessage(), "\n";
    }
}
/*----------------------------------------------------------------*/
/*---------------------------lock part----------------------------*/
function node_lock_form($form , &$form_state , $nid){
    try {
        $data = db_select("desktop_lock_encryption" , "encryption")
            ->fields("encryption")->condition("nid" , $nid)->orderBy("id" , "DESC")->execute()->fetchAll();

        $head = array("row" , "title" , "name" , "new_name" , "uid" , "fid" , "active" , "description" , "delete");
        $form['lock_head'] = array('#type' => 'fieldset');
        foreach($head as $key=>$value){
            $form['lock_head'][$key] = array(
                '#type' => 'item',
                '#markup' => $value,
            );
        }
        $form['lock_count'] = array('#type' => 'hidden', '#value' => count($data));
        foreach ($data as $key => $value){
            $form['lock_'. $key] = array('#type' => 'fieldset',);
            $form['lock_'. $key]['row_' . $key] = array(
                '#type' => 'item',
                '#markup' => $key,
            );
            $form['lock_'. $key]['title_' . $key] = array(
                '#type' => 'item',
                '#markup' => $value->title,
            );
            $form['lock_'. $key]['name_' . $key] = array(
                '#type' => 'item',
                '#markup' => $value->name,
            );
            $form['lock_'. $key]['new_name_' . $key] = array(
                '#type' => 'item',
                '#markup' => $value->new_name,
            );
            $form['lock_'. $key]['uid_' . $key] = array(
                '#type' => 'item',
                '#markup' => $value->uid,
            );
            $form['lock_'. $key]['fid_' . $key] = array(
                '#type' => 'item',
                '#markup' => $value->fid,
            );
            $form['lock_'. $key]['active_' . $key] = array(
                '#type' => 'select',
                '#options' => array(0 => "No" , 1 => "Yes"),
                '#default_value' => $value->active,
            );
            $form['lock_'. $key]['description_' . $key] = array(
                '#type' => 'textfield',
                '#default_value' => empty($value->description) ? "" : $value->description,
            );
            $form['lock_'. $key]['delete_' . $key] = array(
                '#type' =>'checkbox',
            );

            $form['lock_'. $key]['id_' . $key] = array('#type' => 'hidden', '#value' => $value->id);
        }
        $form['submit'] = array(
            '#type' => 'submit',
            '#value' => "save"
        );

        return $form;

    } catch (Exception $e) {
        echo 'Caught exception: ',  $e->getMessage(), "\n";
    }
}
function node_lock_form_validate($form , &$form_state){

}
function node_lock_form_submit($form , &$form_state){
    try {
        $results = array();
        $old_results = array();
        for($i = 0; $i < $form_state['values']['lock_count']; $i++){
            $results[$i]["id"] = $form_state['values']['id_' . $i];
            $results[$i]["change"] = false;
            $results[$i]["delete"] = false;

            if($form_state['values']['delete_' . $i] == 1){
                $query = db_select("desktop_lock_encryption" , "locked");
                $query->join("field_data_field_locked_files" , "file" , "file.field_locked_files_fid = locked.fid and file.entity_id = locked.nid");
                $query->fields("locked" , array("nid" , "fid"));
                $query->condition("locked.id" , $form_state['values']['id_' . $i]);
                $files = $query->execute()->fetch();
                if(empty($files)) $results[$i]["delete"] = true;
                else drupal_set_message("row " . $i . " can not deleted because the file is exist on node");
            }


            $key = array("active" , "description");
            $old_results[$i] = array(
                "active" => $form["lock_" . $i]['active_' . $i]["#default_value"],
                "description" => $form["lock_" . $i]['description_' . $i]["#default_value"],
            );
            $results[$i]["active"] =  $form_state['values']['active_' . $i];
            $results[$i]["description"] =  $form_state['values']['description_' . $i];

            foreach ($key as $item){
                if($results[$i][$item] != $old_results[$i][$item]) {
                    $results[$i]["change"] = true;
                    break;
                }
            }
        }

        foreach ($results as $result){
            if($result["delete"] == true){
                db_delete("desktop_lock_encryption")->condition("id" , $result["id"])->execute();
            }
            elseif($result["change"] == true) {
                db_merge("desktop_lock_encryption")
                    ->key(array("id" => $result["id"]))
                    ->fields(array(
                        'active' => $result["active"],
                        'description' => $result["description"],
                        'updated' => REQUEST_TIME,
                    ))->execute();
            }
        }

    } catch (Exception $e) {
        echo 'Caught exception: ',  $e->getMessage(), "\n";
    }
}
/*----------------------------------------------------------------*/
/*-------------applications log table for admin-------------------*/
function log_form($form, &$form_state){
    try {
        /*get the default value for filtering from url parameters*/
        $default = $_GET;
        /*this fieldset is used for filter part*/
        $form['filter'] = array('#type' => 'fieldset');

        /*fetch the list of actions for filter the actions*/
        $query = db_select("app_log" , "log")->distinct();
        $query->fields("log" , array("action"));
        $query->condition("log.action" , "login" , "<>");
        $query->condition("log.action" , "encrypting" , "<>");
        $result = $query->execute()->fetchCol();
        $action = array("all" => "all");
        foreach ($result as $value){
            if(!empty($value) && strlen($value) > 0) $action[$value] = $value;
        }
        $form['filter']['action'] = array(
            '#type' => 'select',
            '#title' => 'action',
            '#options' => $action,
            '#default_value' => empty($default['action']) ? array("all" => "all") : array($default['action'] => $default['action']),
        );

        /*fetch the list of entity_types for filter the entity_types*/
        $query = db_select("app_log" , "log")->distinct();
        $query->fields("log" , array("entity_type"));
        $result = $query->execute()->fetchCol();
        $entity_type = array("all" => "all");
        foreach ($result as $value){
            if(!empty($value) && strlen($value) > 0) $entity_type[$value] = $value;
        }
        $form['filter']['entity_type'] = array(
            '#type' => 'select',
            '#title' => 'type',
            '#options' => $entity_type,
            '#default_value' => empty($default['entity_type']) ? array("all" => "all") : array($default['entity_type'] => $default['entity_type']),
        );

        /*fetch the list of app_names for filter the app_names*/
        $query = db_select("app_log" , "log")->distinct();
        $query->fields("log" , array("app_name"));
        $result = $query->execute()->fetchCol();
        $app_name = array("all" => "all");
        foreach ($result as $value){
            if(!empty($value) && strlen($value) > 0) $app_name[$value] = $value;
        }
        $form['filter']['app_name'] = array(
            '#type' => 'select',
            '#title' => 'app',
            '#options' => $app_name,
            '#default_value' => empty($default['app_name']) ? array("all" => "all") : array($default['app_name'] => $default['app_name']),
        );

        /*fetch the list of app_versions for filter the app_versions*/
        $query = db_select("app_log" , "log")->distinct();
        $query->fields("log" , array("app_version"));
        $result = $query->execute()->fetchCol();
        $app_version = array("all" => "all");
        foreach ($result as $value){
            if(!empty($value) && strlen($value) > 0) $app_version[$value] = $value;
        }
        $form['filter']['app_version'] = array(
            '#type' => 'select',
            '#title' => 'version',
            '#options' => $app_version,
            '#default_value' => empty($default['app_version']) ? array("all" => "all") : array($default['app_version'] => $default['app_version']),
        );

        /*fetch the list of statuses for filter the statuses*/
        $query = db_select("app_log" , "log")->distinct();
        $query->fields("log" , array("status"));
        $result = $query->execute()->fetchCol();
        $status = array("all" => "all");
        foreach ($result as $value){
            if(!empty($value) && strlen($value) > 0) $status[$value] = $value;
        }
        $form['filter']['status'] = array(
            '#type' => 'select',
            '#title' => 'status',
            '#options' => $status,
            '#default_value' => empty($default['status']) ? array("all" => "all") : array($default['status'] => $default['status']),
        );

        $form['filter']['save'] = array(
            '#type' => 'submit',
            '#value' => "اعمال",
        );

        /*fetch the data from app_log table in db
         *show the 30 row for each page
         * filter the data with default values
         */
        $query = db_select("app_log" , "log")->extend('PagerDefault')->extend('TableSort');
        $query->join("users" , "user" , "user.uid = log.uid");
        $query->fields("user" , array("name"));
        $query->fields("log" , array("uid" , "action" , "entity_type" , "entity_id" , "app_name" , "app_version" , "created" , "status"));
        $query->condition("log.action" , "login" , "<>");
        $query->condition("log.action" , "encrypting" , "<>");
        if(!empty($default['action']) && $default['action'] != "all"){
            $query->condition("log.action" , $default['action']);
        }
        if(!empty($default['entity_type']) && $default['entity_type'] != "all"){
            $query->condition("log.entity_type" , $default['entity_type']);
        }
        if(!empty($default['app_name']) && $default['app_name'] != "all"){
            $query->condition("log.app_name" , $default['app_name']);
        }
        if(!empty($default['app_version']) && $default['app_version'] != "all"){
            $query->condition("log.app_version" , $default['app_version']);
        }
        if(!empty($default['status']) && $default['status'] != "all"){
            $query->condition("log.status" , $default['status']);
        }
        $query->orderBy("log.id" , "DESC");
        $query->limit(30);
        $result = $query->execute()->fetchAll();

        /*render the table log*/
        $header = array("row" , "user" , "action" , "entity_type" , "entity_id" , "app" , "version" , "time" , "status");
        $rows = array();
        foreach ($result as $key=>$row) {
            /*linking username to the profile*/
            $row->name = l($row->name , "user/" . $row->uid , array("attributes" => array("target" => "_blank")));
            /*convert timestamp to date*/
            $row->created = format_date($row->created);
            unset($row->uid);
            /*convert object to array for being able to add row number at the first column*/
            $row = (array)$row;
            /*computing the row number based on the page number*/
            $page = isset($default["page"]) ? $default["page"]*30 : 0;
            $num = $key + 1 + $page;
            $row = array("row" => $num) + $row;

            $rows[] = array('data' => $row);
        }
        $form['table'] = array(
            '#markup' => theme('table', array('header' => $header, 'rows' => $rows)) . theme('pager'),
        );

        return $form;
    } catch (Exception $e) {
        echo 'Caught exception: ',  $e->getMessage(), "\n";
    }
}
/* just call the page with filter parameter
 * it sends the filters in the parameter of url
 * */
function log_form_submit($form, &$form_state){
    $default = array("action" , "entity_type" , "app_name" , "app_version" , "status");
    $parameter = array();
    foreach ($default as $key){
        if(!empty($form_state['values'][$key]))$parameter[$key] = $form_state['values'][$key];
    }
    drupal_goto("admin/structure/services/log" , array("query" => $parameter));
}
/*----------------------------------------------------------------*/

/*
 * custom taxonomy term page
 *
 */
function taxonomy_term_page($term){
    if($term->vid == 28){
        $query = db_select("field_data_field_bool" , "field_bool");
        $query->fields("field_bool" , array("field_bool_value"));
        $query->condition("field_bool.entity_type" , "taxonomy_term");
        $query->condition("field_bool.bundle" , "expert");
        $query->condition("field_bool.entity_id" , $term->tid);
        $bool = $query->execute()->fetch();
        global $user;
        if((empty($bool) || $bool->field_bool_value == 0) && !in_array("administrator" , $user->roles)){
            drupal_access_denied();
            module_invoke_all('exit');
            drupal_exit();
        }
    }
    // If there is a menu link to this term, the link becomes the last part of
    // the active trail, and the link name becomes the page title. Thus, we must
    // explicitly set the page title to be the term title.
    drupal_set_title($term->name);
    // Build breadcrumb based on the hierarchy of the term.
    $current = (object) array(
        'tid' => $term->tid,
    );
    // @todo This overrides any other possible breadcrumb and is a pure hard-coded
    //   presumption. Make this behavior configurable per vocabulary or term.
    $breadcrumb = array();
    while ($parents = taxonomy_get_parents($current->tid)) {
        $current = array_shift($parents);
        $breadcrumb[] = l($current->name, 'taxonomy/term/' . $current->tid);
    }
    $breadcrumb[] = l(t('Home'), NULL);
    $breadcrumb = array_reverse($breadcrumb);
    drupal_set_breadcrumb($breadcrumb);
    drupal_add_feed('taxonomy/term/' . $term->tid . '/feed', 'RSS - ' . $term->name);
    // Set the term path as the canonical URL to prevent duplicate content.
    $uri = entity_uri('taxonomy_term', $term);
    drupal_add_html_head_link(array('rel' => 'canonical', 'href' => url($uri['path'], $uri['options'])), TRUE);
    // Set the non-aliased path as a default shortlink.
    drupal_add_html_head_link(array('rel' => 'shortlink', 'href' => url($uri['path'], array_merge($uri['options'], array('alias' => TRUE)))), TRUE);

    // Normally we would call taxonomy_term_show() here, but for backwards
    // compatibility in Drupal 7 we do not want to do that (it produces different
    // data structures and HTML markup than what Drupal 7 released with). Calling
    // taxonomy_term_view() directly provides essentially the same thing, but
    // allows us to wrap the rendered term in our desired array structure.
    $build['term_heading'] = array(
        '#prefix' => '<div class="term-listing-heading">',
        '#suffix' => '</div>',
        'term' => taxonomy_term_view($term, 'full'),
    );

    if($term->vid == 28) {//tags vocabulary
        $query = db_select("taxonomy_index", "t_index");
        $query->join("node", "node", "node.nid = t_index.nid");
        $query->fields("node", array("nid", "type"));
        $query->condition("t_index.tid", $term->tid);
        $query->condition("node.status" , 1);
        $contents = $query->execute()->fetchAll();
        if (!empty($contents)) {
            $nodes = array();
            foreach ($contents as $content) {
                if (!isset($nodes[$content->type])) $nodes[$content->type] = array();
                array_push($nodes[$content->type], $content->nid);
            }
            $sort = array("gallerynew" , "film" , "designteam" , "article" , "podcast" , "publication" , "blog" , "education" , "college" , "product" , "questions");
            $sorted = array();
            foreach ($sort as $type) {
                if(!empty($nodes[$type]))
                    $sorted[$type] = $nodes[$type];
            }
            foreach ($sorted as $type => $nids) {
                $name = db_select("node_type")->fields("node_type", array("name"))->condition("type", $type)->execute()->fetch()->name;
                $view_mode = "question";
                if (
                    strcmp($type, "gallerynew") == 0 ||
                    strcmp($type, "film") == 0 ||
                    strcmp($type, "questions") == 0 ||
                    strcmp($type, "product") == 0
                ) {
                    $view_mode = "teaser";
                }
                $nodes = node_load_multiple($nids);
                if (!isset($build[$type])) $build[$type] = array();
                $build[$type]["#prefix"] = '<div id="' . $type . '" class="node-type-relative"><h2 class="field-label">' . $name . '</h2><div class="field-items">';
                $build[$type]["#suffix"] = '</div></div>';
                $build[$type]["term"] = node_view_multiple($nodes, $view_mode);
            }

        }
        else {
            $build['no_content'] = array(
                '#prefix' => '<p>',
                '#markup' => t('There is currently no content classified with this term.'),
                '#suffix' => '</p>',
            );
        }

        //add users of taxonomy term which isn't provided by default
	    $build["experts"] = array(
		    "#prefix" => '<div id="advisors" class="node-type-relative"><h2 class="field-label">مشاوران این تخصص</h2><div class="field-items">',
		    "#suffix" => views_embed_view('user_experts', 'pedia_experts_advisors', $term->tid) .'</div></div>',
		    "term" => ''
	    );
	    $build["users"] = array(
		    "#prefix" => '<div id="users" class="node-type-relative"><h2 class="field-label">کاربران</h2><div class="field-items">',
		    "#suffix" => views_embed_view('user_experts', 'pedia_experts_users', $term->tid) .'</div></div>',
		    "term" => '',
	    );
    }else{ //default taxonomy terms page output
        if ($nids = taxonomy_select_nodes($term->tid, TRUE, variable_get('default_nodes_main', 10))) {
            $nodes = node_load_multiple($nids);
            $build += node_view_multiple($nodes);
            $build['pager'] = array(
                '#theme' => 'pager',
                '#weight' => 5,
            );
        }
        else {
            $build['no_content'] = array(
                '#prefix' => '<p>',
                '#markup' => t('There is currently no content classified with this term.'),
                '#suffix' => '</p>',
            );
        }
    }
    return $build;
}